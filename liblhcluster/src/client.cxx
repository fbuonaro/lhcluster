#include <lhcluster_impl/client.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

namespace LHClusterNS
{
namespace Impl
{
    const char* ClientConstructionFailed::what() const throw()
    {
        return "Failed to construct client";
    }

    Client::Client(
        const Endpoint& _clientMessagingEndpoint,
        const Endpoint& _clientProxyMessagingEndpoint,
        std::unique_ptr< IZMQSocketFactory > _socketFactory,
        std::unique_ptr< IZMQSocketSenderReceiver > 
            _senderReceiver )
    :   IClient()
    ,   clientMessagingEndpoint( _clientMessagingEndpoint )
    ,   clientProxyMessagingEndpoint( _clientProxyMessagingEndpoint )
    ,   clientMessagingSocket( _socketFactory->Create( SocketType::Dealer ) )
    ,   clientMessagingPoller( nullptr )
    ,   socketFactory( std::move( _socketFactory ) )
    ,   senderReceiver( std::move( _senderReceiver ) )
    {
        if( !( clientMessagingSocket ) )
            throw ClientConstructionFailed();

        clientMessagingPoller = zpoller_new(
            clientMessagingSocket, NULL );
        if( !( clientMessagingPoller ) )
            throw ClientConstructionFailed();
    }

    Client::~Client()
    {
        if( clientMessagingPoller )
            zpoller_destroy( &clientMessagingPoller );

        if( clientMessagingSocket )
        {
            Deactivate();
            socketFactory->Destroy( &clientMessagingSocket );
            clientMessagingSocket = nullptr;
        }
    }

    int Client::Activate()
    {
        int ret = 0;

        ret = socketFactory->Bind(
            clientMessagingSocket, clientMessagingEndpoint.path() ) < 0;
        ret += socketFactory->Connect(
            clientMessagingSocket, clientProxyMessagingEndpoint.path() ) < 0;

        return ret;
    }

    int Client::Deactivate()
    {
        int ret = 0;

        ret = socketFactory->DisConnect(
            clientMessagingSocket, clientProxyMessagingEndpoint.path() );
        ret = socketFactory->UnBind(
            clientMessagingSocket, clientMessagingEndpoint.path() );

        return ret;
    }

    int Client::SendAsynchronousRequest(
        RequestType requestType,
        RequestId requestId )
    {
        return SendRequest(
            requestType,
            requestId,
            MessageFlag::Asynchronous );
    }

    int Client::SendSynchronousRequest(
        RequestType requestType,
        RequestId requestId )
    {
        return SendRequest(
            requestType,
            requestId,
            MessageFlag::None );
    }

    int Client::SendRequest(
        RequestType requestType,
        RequestId requestId,
        MessageFlags messageFlags )
    {
        ZMQMessage* request( zmsg_new( ) );
        int ret = 0;

        if( !request )
            return 1;

        ret = SendRequest( requestType, requestId, messageFlags, &request );
        if( ret && request )
            zmsg_destroy( &request );
        return ret;
    }

    int Client::SendAsynchronousRequest(
        RequestType requestType,
        RequestId requestId,
        ZMQMessage** lpZMQMessage )
    {
        return SendRequest(
            requestType,
            requestId,
            MessageFlag::Asynchronous,
            lpZMQMessage );
    }

    int Client::SendSynchronousRequest(
        RequestType requestType,
        RequestId requestId,
        ZMQMessage** lpZMQMessage )
    {
        return SendRequest(
            requestType,
            requestId,
            MessageFlag::None,
            lpZMQMessage );
    }

    int Client::SendRequest(
        RequestType requestType,
        RequestId requestId,
        MessageFlags messageFlags,
        ZMQMessage** lpZMQMessage )
    {
        ZMQFrameHandler frameHandler;
        ZMQMessage* zmqMessage = *lpZMQMessage;
        LHCVersionFlags vfs( messageFlags );

        frameHandler.prepend_message_value( zmqMessage, requestId );
        frameHandler.prepend_message_value( zmqMessage, requestType );
        frameHandler.prepend_message_value( zmqMessage, vfs );
        frameHandler.prepend_message_value(
            zmqMessage,
            ClusterMessage::ClientNewRequest );
        zmsg_pushmem( zmqMessage, NULL, 0 );
        
        return senderReceiver->Send( lpZMQMessage, clientMessagingSocket );
    }

    int Client::RetrieveResponses(
        std::vector< RequestResponse >& requestResponses,
        int responseLimit )
    {
        return RetrieveResponses( requestResponses, responseLimit, std::chrono::milliseconds( 0 ) );
    }

    int Client::RetrieveResponses(
        std::vector< RequestResponse >& requestResponses,
        int responseLimit,
        std::chrono::seconds timeout_s )
    {
        return RetrieveResponses(
            requestResponses,
            responseLimit,
            std::chrono::duration_cast< std::chrono::milliseconds >(
                timeout_s ) );
    }

    int Client::RetrieveResponses(
        std::vector< RequestResponse >& requestResponses,
        int responseLimit,
        std::chrono::milliseconds timeout_ms )
    {
        auto previousTime( std::chrono::high_resolution_clock::now() );
        std::chrono::milliseconds timeRemaining( timeout_ms );
        int numReceived = 0;

        do
        {
            // assume that only waiting on one socket at the moment
            if( zpoller_wait( clientMessagingPoller, timeRemaining.count() ) )            
            {
                do
                {
                    ZMQMessage* responseMessage(
                        senderReceiver->Receive( clientMessagingSocket ) );
                    requestResponses.emplace_back( &responseMessage );
                    ++numReceived;
                } while( ( numReceived < responseLimit ) && zpoller_wait( clientMessagingPoller, 0 ) );
            }

            // reduce time remaining
            auto currentTime( std::chrono::high_resolution_clock::now() );
            timeRemaining -= std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - previousTime );
            previousTime = currentTime;
        } while( ( numReceived < responseLimit ) && ( timeRemaining.count() > 0 ) );

        return numReceived;
    }
}
}
