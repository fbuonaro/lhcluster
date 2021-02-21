#ifndef __LHCLUSTER_IMPL_CLIENT_H__
#define __LHCLUSTER_IMPL_CLIENT_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/iclient.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>

#include <memory>
#include <boost/optional.hpp>

namespace LHClusterNS
{
namespace Impl
{
    struct ClientConstructionFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class Client : public IClient
    {
        public:
            // DATA MEMBERS
            Endpoint clientMessagingEndpoint;
            Endpoint clientProxyMessagingEndpoint;
            SocketDealer* clientMessagingSocket;
            zpoller_t* clientMessagingPoller;
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;

            //CTOR, DTOR
            Client( const Client& other ) = delete;
            Client( Client&& ) = delete;
            Client& operator=( const Client& other ) = delete;
            Client& operator=( Client&& other ) = delete;
            Client() = delete;

            Client( const Endpoint& _clientMessagingEndpoint,
                    const Endpoint& _clientProxyMessagingEndpoint,
                    std::unique_ptr< IZMQSocketFactory > _socketFactory,
                    std::unique_ptr< IZMQSocketSenderReceiver > 
                        _senderReceiver );
            ~Client();

            //METHODS
            int Activate();
            int Deactivate();
            // IClient
            //Full non-blocking
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit );
            // IClient
            //Block for timeout_s seconds
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::seconds timeout_s );
            // IClient
            //Block for timeout_ms seconds
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::milliseconds timeout_ms );
            // IClient
            int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId );
            // IClient
            int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId );
            // IClient
            int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags );
            // IClient
            int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage );
            // IClient
            int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage );
            // IClient
            int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags,
                ZMQMessage** lpZMQMessage );
    };
}
}

#endif
