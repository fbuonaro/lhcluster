#include <lhcluster_impl/clientproxy.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "test.h"
#include "testzmqsocketutils.h"

#include <vector>
#include <string>
#include <gtest/gtest.h>

namespace TestLHClusterNS
{

    using namespace LHClusterNS;
    using namespace std;

    class TestClientProxy : public ::testing::Test
    {
        protected:

            void SetUp()
            {

            }

            void Tear()
            {
            }
    };

    ZMQMessage* createClientMessage(
        ZMQFrameHandler& frameHandler,
        const char* identity,
        bool includeNullDelimiter,
        const ClusterMessage* clientMessage,
        const LHCVersionFlags* vfs,
        const RequestType* requestType,
        const RequestId* requestId )
    {
        ZMQMessage* msg = zmsg_new();

        if( identity )
            frameHandler.append_message_value( msg, string( identity ) );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( includeNullDelimiter )
            zmsg_addmem( msg, NULL, 0 );

        if( clientMessage )
            frameHandler.append_message_value( msg, *clientMessage );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        return msg;
    }

    // A Client Request message consists of
    // 1) A frame containing the identity
    // 2) A frame containing a null delimiter
    // 3) A frame containing a client message type (unparsed)
    // 4) A frame containing a request type
    // 5) A frame containing a request id
    // 6) zero or more frames containing data
    // Cases: 
    //  send returns non zero
    //  receive returns null message
    //  null identity frame
    //  missing null delimiter
    //  null request type
    //  null request id
    //  msg is only 0 to 4 frames
    //  msg has no data frames
    //  msg has data frames
    //  duplicate identity/requestid/requesttype
    // In all cases, fail or succeed, the input message must be destroyed
    // Only on success will the request be buffered should buffering be
    // enabled
    TEST_F( TestClientProxy, TestHandleClientRequest )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );
        ZMQFrameHandler frameHandler;
        ClusterMessage clientMessagingType = ClusterMessage::ClientNewRequest;
        string identity1( "identity1" );
        RequestId requestId1 = 8953;
        RequestType requestType1 = 12;
        LHCVersionFlags vfs1( 1, 0, (MessageFlag::Asynchronous | MessageFlag::AllowRedirect ) );
        string identity2( "identity2" );
        RequestId requestId2 = 28953;
        RequestType requestType2 = 212;
        LHCVersionFlags vfs2( 1, 0, (MessageFlag::Asynchronous | MessageFlag::NotifyOnRedirect) );
        string identity3( "identity3" );
        RequestId requestId3 = 18953;
        RequestType requestType3 = 112;
        LHCVersionFlags vfs3(
            200,
            3000,
            ( MessageFlag::ReceiptOnReceive ) );
        LHCVersionFlags vfs4(
            20,
            300,
            ( MessageFlag::ReceiptOnSend ) );

        ZMQMessage* justIdentity = nullptr;
        ZMQMessage* noDelimiter = nullptr;
        ZMQMessage* noClientMessageType = nullptr;
        ZMQMessage* noLHCVersionFlags = nullptr;
        ZMQMessage* noRequestType = nullptr;
        ZMQMessage* noRequestId = nullptr;
        ZMQMessage* goodMsg = nullptr;
        ZMQMessage* goodMsgDup = nullptr;
        ZMQMessage* goodMsgPlus = nullptr;
        ZMQMessage* goodMsgPlusDup = nullptr;

        justIdentity = createClientMessage(
            frameHandler,
            identity1.c_str(),
            false,
            nullptr,
            nullptr,
            nullptr,
            nullptr );

        noDelimiter = createClientMessage(
            frameHandler,
            identity1.c_str(),
            false,
            &clientMessagingType,
            &vfs1,
            &requestType1,
            &requestId1 );
 
        noClientMessageType = createClientMessage(
            frameHandler,
            identity1.c_str(),
            true,
            nullptr,
            &vfs1,
            &requestType1,
            &requestId1 );

        noLHCVersionFlags = createClientMessage(
            frameHandler,
            identity1.c_str(),
            true,
            &clientMessagingType,
            nullptr,
            &requestType1,
            &requestId1 );

        noRequestType = createClientMessage(
            frameHandler,
            identity1.c_str(),
            true,
            &clientMessagingType,
            &vfs1,
            nullptr,
            &requestId1 );

         noRequestId = createClientMessage(
            frameHandler,
            identity1.c_str(),
            true,
            &clientMessagingType,
            &vfs1,
            &requestType1,
            nullptr );

        goodMsg = createClientMessage(
            frameHandler,
            identity2.c_str(),
            true,
            &clientMessagingType,
            &vfs2,
            &requestType2,
            &requestId2 );

        goodMsgDup = createClientMessage(
            frameHandler,
            identity2.c_str(),
            true,
            &clientMessagingType,
            &vfs4,
            &requestType2,
            &requestId2 );


        goodMsgPlus = createClientMessage(
            frameHandler,
            identity3.c_str(),
            true,
            &clientMessagingType,
            &vfs3,
            &requestType3,
            &requestId3 );
        frameHandler.append_message_value< string >( goodMsgPlus, "hello" );
        frameHandler.append_message_value< RequestId >( goodMsgPlus, 1 );
        frameHandler.append_message_value< string >( goodMsgPlus, "world" );


        goodMsgPlusDup = zmsg_dup( goodMsgPlus );

        senderReceiver.sendReturnValues = { 1, 0 }; // first send will fail
        senderReceiver.receiveReturnValues = {
            // fail to send, nothing buffered, failed to send stat
            goodMsg,
            // fail to receive
            nullptr,    
            // malformed, nothing sent, nothing buffered
            justIdentity,
            // malformed, nothing sent, nothing buffered
            noDelimiter,
            // malformed, nothing sent, nothing buffered
            noClientMessageType,
            // malformed, nothing sent, nothing buffered
            noLHCVersionFlags,
            // malformed, nothing sent, nothing buffered
            noRequestType,
            // malformed, nothing sent, nothing buffered
            noRequestId,
            // message sent, buffered
            goodMsgDup,
            // message sent, buffered
            goodMsgPlus,
            // message sent, buffer fails because duplicate
            goodMsgPlusDup
        };
         
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );
        int ret = 0;

        // fail to send
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 0,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );

        // fail to receive
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );

        // missing identity
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);

        // missing null delim
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 2,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);

        // missing message type
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 3,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);

        // missing version flags
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 4,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);

        // missing rtype
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 5,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);

        // missing rid
        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 6,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);
        // buffer before sending
        ASSERT_EQ( 0, clientProxy.requestsBuffer.UnderlyingBuffer().size() );

        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSent ] );
        ASSERT_EQ( 1, clientProxy.requestsBuffer.UnderlyingBuffer().size() );

        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2,
                         clientProxy.stats[ ClientProxyStat::RequestsSent ] );
        ASSERT_EQ( 2, clientProxy.requestsBuffer.UnderlyingBuffer().size() );

        ret = clientProxy.handleClientMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3,
                         clientProxy.stats[ ClientProxyStat::RequestsSent ] );
        ASSERT_EQ( 3, clientProxy.requestsBuffer.UnderlyingBuffer().size() );
        ASSERT_EQ( 4, senderReceiver.sentMessages.size() ); // 3 good + 1st bad

        for( auto it = clientProxy.requestsBuffer.UnderlyingBuffer().begin();
             it != clientProxy.requestsBuffer.UnderlyingBuffer().end();
             ++it )
        {
            auto key = it->first;
            auto val = std::get< 2 >( it->second );

            switch( std::get<1>( key ) )
            {
                case( 28953 ):
                {
                    ASSERT_EQ( requestType2, std::get< 0 >( key ) );
                    ASSERT_EQ( identity2,
                               frameHandler.get_frame_value< string >(
                                   std::get< 2 >( key ) ) );
                    ASSERT_EQ( 6, zmsg_size( val ) );
                    break;
                }
                case( 18953 ):
                {
                    ASSERT_EQ( requestType3, std::get< 0 >( key ) );
                    ASSERT_EQ( identity3,
                               frameHandler.get_frame_value< string >(
                                std::get< 2 >( key ) ) );
                    ASSERT_EQ( 9, zmsg_size( val ) );
                    break;
                }
                default:
                {
                    ASSERT_TRUE( false ) << "unexpected buffered request";
                    break;
                }
            }

        }

        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::FailedClientReceives] );
        STATS_ASSERT_EQ( 1,
                         clientProxy.stats[ ClientProxyStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 6,
                         clientProxy.stats[ClientProxyStat::MalformedClientRequests]);
        }
    }

    TEST_F( TestClientProxy, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        TestZMQSocket* brokerSocket = nullptr;
        TestZMQSocket* clientSocket = nullptr;
        TestZMQSocket* listenerSocket = nullptr;
        TestZMQSocket* signalerSocket = nullptr;
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );
        Delay expectedBrokerDeathDelay( 2*selfHBDelay.count() );
        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );

        // should connect to broker ep, bind to self broker facing ep
        brokerSocket = ( TestZMQSocket* ) clientProxy.brokerMessagingSocket;
        clientProxy.activateBrokerMessagingSockets();
        ASSERT_EQ( SocketType::Dealer, brokerSocket->socketType );
        ASSERT_EQ( 1, brokerSocket->binded.size() );
        ASSERT_EQ( 0, brokerSocket->unBinded.size() );
        ASSERT_TRUE( brokerSocket->binded.find(
                        selfBrokerFacingEndpoint.path() ) !=
                     brokerSocket->binded.end() );
        ASSERT_EQ( 1, brokerSocket->connected.size() );
        ASSERT_EQ( 0, brokerSocket->disConnected.size() );
        ASSERT_TRUE( brokerSocket->connected.find(
                        brokerEndpoint.path() ) !=
                     brokerSocket->connected.end() );

        // should bind to self client facing ep
        clientSocket = ( TestZMQSocket* ) clientProxy.clientMessagingSocket;
        clientProxy.activateClientMessagingSockets();
        ASSERT_EQ( SocketType::Router, clientSocket->socketType );
        ASSERT_EQ( 1, clientSocket->binded.size() );
        ASSERT_EQ( 0, clientSocket->unBinded.size() );
        ASSERT_TRUE( clientSocket->binded.find(
                        selfClientFacingEndpoint.path() ) !=
                     clientSocket->binded.end() );

        // should bind to controller endpoint
        listenerSocket = ( TestZMQSocket* ) clientProxy.listenerSocket;
        // should connect to controller endpoint
        signalerSocket = ( TestZMQSocket* ) clientProxy.signalerSocket;
        clientProxy.activateControllerSockets();
        ASSERT_EQ( SocketType::Pair, listenerSocket->socketType );
        ASSERT_EQ( 1, listenerSocket->binded.size() );
        ASSERT_EQ( 0, listenerSocket->unBinded.size() );
        ASSERT_TRUE( listenerSocket->binded.find(
                        selfControllerEndpoint.path() ) !=
                     listenerSocket->binded.end() );
        ASSERT_EQ( SocketType::Pair, signalerSocket->socketType );
        ASSERT_EQ( 1, signalerSocket->connected.size() );
        ASSERT_EQ( 0, signalerSocket->disConnected.size() );
        ASSERT_TRUE( signalerSocket->connected.find(
                        selfControllerEndpoint.path() ) !=
                     signalerSocket->connected.end() );

        // only called when exiting StartAsPrimary, must call manually
        clientProxy.deactivateBrokerMessagingSockets();
        clientProxy.deactivateClientMessagingSockets();

        ASSERT_EQ( selfHBDelay, clientProxy.selfHeartbeatDelay );
        ASSERT_EQ( initBrokerHBDelay, clientProxy.brokerHeartbeatDelay );
        ASSERT_EQ( brokerPulseDelay, clientProxy.brokerPulseDelay );
        ASSERT_EQ( expectedBrokerDeathDelay, clientProxy.brokerDeathDelay );
        }


        ASSERT_TRUE( brokerSocket->destroyed );
        ASSERT_EQ( 1, brokerSocket->binded.size() );
        ASSERT_EQ( 1, brokerSocket->unBinded.size() );
        ASSERT_TRUE( brokerSocket->unBinded.find(
                        selfBrokerFacingEndpoint.path() ) !=
                     brokerSocket->unBinded.end() );
        ASSERT_EQ( 1, brokerSocket->connected.size() );
        ASSERT_EQ( 1, brokerSocket->disConnected.size() );
        ASSERT_TRUE( brokerSocket->disConnected.find(
                        brokerEndpoint.path() ) !=
                     brokerSocket->disConnected.end() );

        ASSERT_TRUE( clientSocket->destroyed );
        ASSERT_EQ( 1, clientSocket->binded.size() );
        ASSERT_EQ( 1, clientSocket->unBinded.size() );
        ASSERT_TRUE( clientSocket->unBinded.find(
                        selfClientFacingEndpoint.path() ) !=
                     clientSocket->unBinded.end() );

        ASSERT_TRUE( listenerSocket->destroyed );
        ASSERT_EQ( 1, listenerSocket->binded.size() );
        ASSERT_EQ( 1, listenerSocket->unBinded.size() );
        ASSERT_TRUE( listenerSocket->unBinded.find(
                        selfControllerEndpoint.path() ) !=
                     listenerSocket->unBinded.end() );
        ASSERT_TRUE( signalerSocket->destroyed );
        ASSERT_EQ( 1, signalerSocket->connected.size() );
        ASSERT_EQ( 1, signalerSocket->disConnected.size() );
        ASSERT_TRUE( signalerSocket->disConnected.find(
                        selfControllerEndpoint.path() ) !=
                     signalerSocket->disConnected.end() );
    }

    // To Test:
    // 4) handleBrokerMessage dispatches to the right place
    // 5) (through dispatcher) handleBrokerHeartbeat
    // 6) (through dispatcher) handleBrokerReceipt
    // 7) (through dispatcher) handleBrokerResponse

    // current time should be updated on call to brokerReceiveMessage
    // All types of broker messages should result in brokerLastActive
    // being updated to current time
    // everything should return 0
    // buffered requests are only removed on receipt
    // responses should only be sent on success
    TEST_F( TestClientProxy, TestHandleBrokerHeartbeat )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );
        ZMQFrameHandler frameHandler;
        std::time_t sentAt1 = 1;
        std::time_t sentAt2 = 20;
        std::time_t sentAt3 = 300;
        Delay newSendDelay1( 1000 );
        Delay newSendDelay2( 2000 );
        Delay newSendDelay3( 3000 );
        ClusterMessage brokerMessage = ClusterMessage::BrokerHeartbeat;
        uint32_t badBrokerMessage = 30000;
        string identity1( "identity1" );
        string identity2( "identity2" );
        string identity3( "identity3" );
        vector< const char* > brokerIdentitiesNull;
        vector< const char* > brokerIdentities1{
            identity1.c_str(),
            identity2.c_str(),
            identity3.c_str()
        };
        vector< const char* > brokerIdentities2{
            identity3.c_str(),
            identity1.c_str(),
            identity2.c_str()
        };
        vector< const char* > brokerIdentities3{
            identity3.c_str(),
            identity3.c_str(),
            identity3.c_str()
        };
        ZMQMessage* goodHeartbeat = nullptr;
        ZMQMessage* noDelim = nullptr;
        ZMQMessage* noMessageType = nullptr;
        ZMQMessage* badMessageType = nullptr;
        ZMQMessage* noSentAt = nullptr;
        ZMQMessage* noNewDelay = nullptr;
        ZMQMessage* goodHeartbeat2 = nullptr;
        ZMQMessage* goodHeartbeat3 = nullptr;

        // cases
        // 1) receive null msg
        // 3) missing null delim
        // 3) no broker message type or bad broker message type
        // 4) missing sent at
        // 5) missing new send delay

        goodHeartbeat = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            &brokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noDelim = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            false,
            &brokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noMessageType = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            nullptr,
            &sentAt1,
            &newSendDelay1 );

        badMessageType = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            ( ClusterMessage* ) &badBrokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noSentAt = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            &brokerMessage,
            nullptr,
            &newSendDelay1 );

        noNewDelay = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            &brokerMessage,
            &sentAt1,
            nullptr );

        goodHeartbeat2 = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            &brokerMessage,
            &sentAt2,
            &newSendDelay2 );


        goodHeartbeat3 = createBrokerHeartbeat(
            frameHandler,
            brokerIdentitiesNull,
            true,
            &brokerMessage,
            &sentAt3,
            &newSendDelay3 );

        senderReceiver.receiveReturnValues = {
            nullptr,
            goodHeartbeat,
            noDelim,
            noMessageType,
            badMessageType,
            noSentAt,
            noNewDelay,
            goodHeartbeat2,
            goodHeartbeat3
        };
         
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );
        int ret = 0;

        // null message, ClientProxyStat::FailedBrokerReceives
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        STATS_ASSERT_EQ( 1, clientProxy.stats[ ClientProxyStat::FailedBrokerReceives ] );

        // good heartbeat, ClientProxyStat::BrokerHeartbeatsReceived,
        // brokerLastActive == currentTime
        // new send delay
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        clientProxy.brokerLastActive = 1;
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::BrokerHeartbeatsReceived ] );
        ASSERT_EQ( newSendDelay1, clientProxy.brokerHeartbeatDelay);

        // noDelim
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        // noMessageType
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        STATS_ASSERT_EQ(
            2,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        // badMessageType
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        STATS_ASSERT_EQ(
            3,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        // noSentAt
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        STATS_ASSERT_EQ(
            4,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        // noNewDelay
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        STATS_ASSERT_EQ(
            5,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        // goodHeartbeat2
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        clientProxy.brokerLastActive = 1;
        ASSERT_EQ( newSendDelay2, clientProxy.brokerHeartbeatDelay );
        STATS_ASSERT_EQ(
            4,
            clientProxy.stats[ ClientProxyStat::BrokerHeartbeatsReceived ] );

        // goodHeartbeat3
        clientProxy.currentTime = 0;
        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( clientProxy.currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( clientProxy.brokerLastActive, clientProxy.currentTime );
        ASSERT_EQ( newSendDelay3, clientProxy.brokerHeartbeatDelay );
        STATS_ASSERT_EQ(
            5,
            clientProxy.stats[ ClientProxyStat::BrokerHeartbeatsReceived ] );
        }
    }

    //  b) If response then
    //  3b) a nonempty identity frame
    //  4b) one null delimiter
    //  5b) request type
    //  6b) request id
    //  7b) additional response frames (if respnose)
    ZMQMessage* createBrokerResponseOrReceipt(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& identities,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage,
        RequestType* requestType,
        RequestId* requestId,
        LHCVersionFlags* vfs
    )
    {
        ZMQMessage* msg = createBrokerMessage(
            frameHandler,
            identities,
            includeNullDelimiter,
            brokerMessage );
        
        if( brokerMessage &&
            *brokerMessage != ClusterMessage::BrokerReceivedReceipt &&
            *brokerMessage != ClusterMessage::BrokerSentReceipt )
        {
            if( vfs )
                frameHandler.append_message_value( msg, *vfs );
            else
            {
                zmsg_addmem( msg, NULL, 0 );
                return msg;
            }
        }

        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        return msg;
    }

    TEST_F( TestClientProxy, TestHandleBrokerResponse )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );
        ZMQFrameHandler frameHandler;
        ClusterMessage brokerMessage = ClusterMessage::BrokerResponse;
        string identity1( "identity1" );
        string identity2( "identity2" );
        string identity3( "identity3" );
        vector< const char* > brokerIdentities1{
            identity1.c_str()
        };
        vector< const char* > brokerIdentities2{
            identity2.c_str()
        };
        vector< const char* > brokerIdentities3{
            identity3.c_str()
        };
        vector< const char* > brokerMultiIdentities{
            identity1.c_str(),
            identity2.c_str(),
            identity3.c_str() };
        vector< const char* > brokerNoIdentities;
        RequestId requestId1 = 1;
        RequestId requestId2 = 200;
        RequestId requestId3 = 3000;
        RequestType requestType1 = 5;
        RequestType requestType2 = 8000;
        RequestType requestType3 = 900000;
        LHCVersionFlags vfs1( 999, 888, MessageFlag::None );
        LHCVersionFlags vfs2( MessageFlag::Asynchronous );
        LHCVersionFlags vfs3( MessageFlag::Asynchronous | MessageFlag::AllowRedirect );
        ZMQMessage* goodResponse = nullptr;
        ZMQMessage* noIdentities = nullptr;
        ZMQMessage* multIdentities = nullptr;
        ZMQMessage* noRequestType = nullptr;
        ZMQMessage* noRequestId = nullptr;
        ZMQMessage* goodResponsePlus = nullptr;
        ZMQMessage* goodResponse2 = nullptr;
        ZMQMessage* noVFS = nullptr;

        // cases
        // 1) receive null msg
        // 2) fail to send
        // 2) no identities
        // 3) multiple identities (only one supported atm)
        // 3) missing request type
        // 4) missing request id
        // 5) good
        // 6) good with data
        // 7) no vfs

        goodResponse = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities1, true, &brokerMessage,
            &requestType1, &requestId1, &vfs1 );

        noIdentities = createBrokerResponseOrReceipt(
            frameHandler, brokerNoIdentities, true, &brokerMessage,
            &requestType1, &requestId1, &vfs1 );

        multIdentities = createBrokerResponseOrReceipt(
            frameHandler, brokerMultiIdentities, true, &brokerMessage,
            &requestType1, &requestId1, &vfs1 );

        noRequestType = createBrokerResponseOrReceipt(
            frameHandler, brokerMultiIdentities, true, &brokerMessage,
            nullptr, &requestId1, &vfs1 );

        noRequestId = createBrokerResponseOrReceipt(
            frameHandler, brokerMultiIdentities, true, &brokerMessage,
            &requestType1, nullptr, &vfs1 );

        noVFS = createBrokerResponseOrReceipt(
            frameHandler, brokerMultiIdentities, true, &brokerMessage,
            &requestType1, &requestId1, nullptr );

        goodResponsePlus = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities3, true, &brokerMessage,
            &requestType3, &requestId3, &vfs3 );
        frameHandler.append_message_value< RequestId >( goodResponsePlus, 895310 );
        frameHandler.append_message_value< string >( goodResponsePlus, "data" );

        goodResponse2 = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities2, true, &brokerMessage,
            &requestType2, &requestId2, &vfs2 );

        ASSERT_EQ( 8, zmsg_size( goodResponsePlus ) );
        ASSERT_EQ( 6, zmsg_size( goodResponse2 ) );

        senderReceiver.receiveReturnValues = {
            nullptr,
            goodResponse,   // should fail to send
            noIdentities,
            multIdentities,
            noRequestType,
            noRequestId,
            goodResponsePlus,
            goodResponse2,
            noVFS
        };

        senderReceiver.sendReturnValues = { 1, 0 };
         
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );
        int ret = 0;

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::FailedBrokerReceives ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::ResponsesReceived ] );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::ResponsesSentFailed ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            2,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            3,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            4,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            2,
            clientProxy.stats[ ClientProxyStat::ResponsesReceived ] );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::ResponsesSent ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            3,
            clientProxy.stats[ ClientProxyStat::ResponsesReceived ] );
        STATS_ASSERT_EQ(
            2,
            clientProxy.stats[ ClientProxyStat::ResponsesSent ] );

        ret = clientProxy.handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ(
            5,
            clientProxy.stats[ ClientProxyStat::MalformedBrokerMessages ] );

        ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
        // message type is removed, so there should be one less frame
        // than what we started with
        ASSERT_EQ( 7, zmsg_size( senderReceiver.sentMessages[1].second ) );
        ASSERT_EQ( 5, zmsg_size( senderReceiver.sentMessages[2].second ) );
        ASSERT_EQ( ( TestZMQSocket* ) clientProxy.clientMessagingSocket,
                   senderReceiver.sentMessages[1].first );
        ASSERT_EQ( ( TestZMQSocket* ) clientProxy.clientMessagingSocket,
                   senderReceiver.sentMessages[2].first );
        ASSERT_EQ( nullptr,
                   senderReceiver.sentMessages[0].second );
        ASSERT_EQ( identity3,
                   frameHandler.get_frame_value< string >(
                       zmsg_first( senderReceiver.sentMessages[1].second ) ) );
        ASSERT_EQ( identity2,
                   frameHandler.get_frame_value< string >(
                       zmsg_first( senderReceiver.sentMessages[2].second ) ) );
        zmsg_next( senderReceiver.sentMessages[1].second );
        zmsg_next( senderReceiver.sentMessages[2].second );
        ASSERT_EQ( vfs3,
                   frameHandler.get_frame_value< LHCVersionFlags >(
                       ( zmsg_next( senderReceiver.sentMessages[1].second ) ) ) );
        ASSERT_EQ( vfs2,
                   frameHandler.get_frame_value< LHCVersionFlags >(
                       ( zmsg_next( senderReceiver.sentMessages[2].second ) ) ) );
        ASSERT_EQ( requestType3,
                   frameHandler.get_frame_value< RequestType >(
                       ( zmsg_next( senderReceiver.sentMessages[1].second ) ) ) );
        ASSERT_EQ( requestType2,
                   frameHandler.get_frame_value< RequestType >(
                       ( zmsg_next( senderReceiver.sentMessages[2].second ) ) ) );
        ASSERT_EQ( requestId3,
                   frameHandler.get_frame_value< RequestId >(
                       ( zmsg_next( senderReceiver.sentMessages[1].second ) ) ) );
        ASSERT_EQ( requestId2,
                   frameHandler.get_frame_value< RequestId >(
                       ( zmsg_next( senderReceiver.sentMessages[2].second ) ) ) );
        }
    }

    TEST_F( TestClientProxy, TestReceiveBrokerReceipt )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );
        ZMQFrameHandler frameHandler;
        ClusterMessage brokerMessageRecvReceipt = ClusterMessage::BrokerReceivedReceipt;
        ClusterMessage brokerMessageSentReceipt = ClusterMessage::BrokerSentReceipt;
        string identity1( "identity1" );
        string identity2( "identity2" );
        string identity3( "identity3" );
        vector< const char* > brokerIdentities1{
            identity1.c_str()
        };
        vector< const char* > brokerIdentities2{
            identity2.c_str()
        };
        vector< const char* > brokerIdentities3{
            identity3.c_str()
        };
        vector< const char* > brokerMultiIdentities{
            identity1.c_str(),
            identity2.c_str(),
            identity3.c_str() };
        vector< const char* > brokerNoIdentities;
        RequestId requestId1 = 1;
        RequestId requestId2 = 200;
        RequestId requestId3 = 3000;
        RequestType requestType1 = 5;
        RequestType requestType2 = 8000;
        RequestType requestType3 = 900000;
        ZMQMessage* goodReceipt = nullptr;
        ZMQMessage* goodReceipt2 = nullptr;
        ZMQMessage* goodReceiptCopy = nullptr;
        ZMQMessage* goodReceipt2Copy = nullptr;
        ZMQMessage* goodReceipt3 = nullptr;
        LHCVersionFlags vfs1( MessageFlag::ReceiptOnReceive );
        LHCVersionFlags vfs2( MessageFlag::ReceiptOnSend );
        LHCVersionFlags vfs3( MessageFlag::ReceiptOnSend | MessageFlag::ReceiptOnSend );

        goodReceipt = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities1, true, &brokerMessageRecvReceipt,
            &requestType1, &requestId1, nullptr );

        goodReceipt2 = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities2, true, &brokerMessageSentReceipt,
            &requestType2, &requestId2, nullptr );

        goodReceipt3 = createBrokerResponseOrReceipt(
            frameHandler, brokerIdentities2, true, &brokerMessageRecvReceipt,
            &requestType3, &requestId3, nullptr );

        goodReceiptCopy = zmsg_dup( goodReceipt );
        goodReceipt2Copy = zmsg_dup( goodReceipt2 );

        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );
        int ret = 0;

        clientProxy.requestsBuffer.Add(
            requestType1,
            requestId1,
            zmsg_first( goodReceipt ),
            vfs1,
            goodReceiptCopy );

        clientProxy.requestsBuffer.Add(
            requestType2,
            requestId2,
            zmsg_first( goodReceipt2 ),
            vfs2,
            goodReceipt2Copy );

        ASSERT_EQ(
            2,
            clientProxy.requestsBuffer.UnderlyingBuffer().size() );

        ret = clientProxy.handleBrokerMessage( &goodReceipt );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ(
            1,
            clientProxy.requestsBuffer.UnderlyingBuffer().size() );
        STATS_ASSERT_EQ(
            1,
            clientProxy.stats[ ClientProxyStat::ReceiptsReceived ] );

        ret = clientProxy.handleBrokerMessage( &goodReceipt3 );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ(
            1,
            clientProxy.requestsBuffer.UnderlyingBuffer().size() );
        STATS_ASSERT_EQ(
            2,
            clientProxy.stats[ ClientProxyStat::ReceiptsReceived ] );

        ret = clientProxy.handleBrokerMessage( &goodReceipt2 );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ(
            0,
            clientProxy.requestsBuffer.UnderlyingBuffer().size() );
        STATS_ASSERT_EQ(
            3,
            clientProxy.stats[ ClientProxyStat::ReceiptsReceived ] );

        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        }

        // TODO - check that buffer requests' receipts are reset on broker death
    }

    TEST_F( TestClientProxy, TestIsBrokerAliveAsOf )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );

        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );

        clientProxy.brokerLastActive = 10;
        ASSERT_TRUE( clientProxy.isBrokerAliveAsOf( 20 ) );
        ASSERT_TRUE( clientProxy.isBrokerAliveAsOf( 1 ) );
        ASSERT_TRUE( clientProxy.isBrokerAliveAsOf( 35 ) );
        ASSERT_TRUE( clientProxy.isBrokerAliveAsOf( 36 ) );
        ASSERT_FALSE( clientProxy.isBrokerAliveAsOf( 37 ) );
        ASSERT_FALSE( clientProxy.isBrokerAliveAsOf( 37000000 ) );
         
        }
    }

    TEST_F( TestClientProxy, TestHeartbeatNeededAsOf )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint brokerEndpoint( EndpointType::TCP, "broker" );
        Endpoint selfBrokerFacingEndpoint( EndpointType::TCP, "brokerFacing" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint selfClientFacingEndpoint( EndpointType::TCP, "clientFacing" );
        Delay initBrokerHBDelay( 10 );
        Delay selfHBDelay( 13 );
        Delay brokerPulseDelay( 3 );

        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        ClientProxy clientProxy( selfBrokerFacingEndpoint,
                                 brokerEndpoint,
                                 selfControllerEndpoint,
                                 selfClientFacingEndpoint,
                                 selfHBDelay,
                                 initBrokerHBDelay,
                                 brokerPulseDelay,
                                 move( lpSocketFactory ),
                                 move( lpSenderReceiver ) );

        clientProxy.clientLastActive = 10;
        ASSERT_TRUE( clientProxy.heartbeatNeededAsOf( 20 ) );
        ASSERT_FALSE( clientProxy.heartbeatNeededAsOf( 19 ) );
        ASSERT_FALSE( clientProxy.heartbeatNeededAsOf( 1 ) );
        }
    }

}
