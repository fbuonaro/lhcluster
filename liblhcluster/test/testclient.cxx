#include <lhcluster_impl/client.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "testzmqsocketutils.h"
#include "test.h"

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    class TestClient : public ::testing::Test
    {
    protected:

        void SetUp()
        {

        }

        void Tear()
        {
        }
    };

    TEST_F( TestClient, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointMessaging( EndpointType::InterProcess, "messaging" );
        Endpoint endpointProxy( EndpointType::InterProcess, "proxy" );
        TestZMQSocket* clientMessagingSocket = nullptr;

        {
            std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            Client client( endpointMessaging,
                endpointProxy,
                std::move( lpSocketFactory ),
                std::move( lpSenderReceiver ) );

            clientMessagingSocket =
                (TestZMQSocket*)client.clientMessagingSocket;

            client.Activate();

            ASSERT_FALSE( clientMessagingSocket->destroyed );
            ASSERT_EQ( SocketType::Dealer, clientMessagingSocket->socketType );
            ASSERT_TRUE( client.clientMessagingPoller != nullptr );
            ASSERT_EQ( 1, clientMessagingSocket->binded.size() );
            ASSERT_EQ( 0, clientMessagingSocket->unBinded.size() );
            ASSERT_EQ( 1, clientMessagingSocket->connected.size() );
            ASSERT_EQ( 0, clientMessagingSocket->disConnected.size() );
            ASSERT_TRUE( clientMessagingSocket->binded.find( endpointMessaging.path() ) !=
                clientMessagingSocket->binded.end() );
            ASSERT_TRUE( clientMessagingSocket->connected.find( endpointProxy.path() )
                != clientMessagingSocket->connected.end() );
        }

        ASSERT_TRUE( clientMessagingSocket->destroyed );
        ASSERT_EQ( SocketType::Dealer, clientMessagingSocket->socketType );
        ASSERT_EQ( 1, clientMessagingSocket->binded.size() );
        ASSERT_EQ( 1, clientMessagingSocket->unBinded.size() );
        ASSERT_EQ( 1, clientMessagingSocket->connected.size() );
        ASSERT_EQ( 1, clientMessagingSocket->disConnected.size() );
        ASSERT_TRUE( clientMessagingSocket->binded.find(
            endpointMessaging.path() ) !=
            clientMessagingSocket->binded.end() );
        ASSERT_TRUE( clientMessagingSocket->connected.find(
            endpointProxy.path() ) !=
            clientMessagingSocket->connected.end() );
        ASSERT_TRUE( clientMessagingSocket->unBinded.find(
            endpointMessaging.path() ) !=
            clientMessagingSocket->unBinded.end() );
        ASSERT_TRUE( clientMessagingSocket->disConnected.find(
            endpointProxy.path() ) !=
            clientMessagingSocket->disConnected.end() );
    }

#define EXPECT_EXCEPTION( exceptionType, statements ) \
    { \
    bool exceptionThrown = false; \
    try \
    { \
        do{ statements } while( false ); \
    } \
    catch( const exceptionType& e ) \
    { \
        exceptionThrown = true; \
    } \
    catch( const std::exception& e ) \
    { \
        exceptionThrown = true; \
        ASSERT_TRUE( false ) \
        << "Excpected exception " << #exceptionType \
        << " but got " << e.what(); \
    } \
    catch( ... ) \
    {} \
    ASSERT_TRUE( exceptionThrown ) \
    << "Expected exception " << #exceptionType << " not thrown"; \
    }

    void populateResponseGood( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        RequestStatus requestStatus,
        RequestId requestId,
        RequestType requestType,
        const LHCVersionFlags& vfs )
    {
        // good message
        frameHandler.prepend_message_value( response, requestStatus );
        frameHandler.prepend_message_value( response, requestId );
        frameHandler.prepend_message_value( response, requestType );
        frameHandler.prepend_message_value( response, vfs );
        zmsg_pushmem( response, NULL, 0 );
    }

    // null type
    void populateResponseBad( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        RequestStatus requestStatus,
        RequestId requestId,
        const LHCVersionFlags& vfs )
    {
        frameHandler.prepend_message_value( response, requestStatus );
        frameHandler.prepend_message_value( response, requestId );
        zmsg_pushmem( response, NULL, 0 );
        frameHandler.prepend_message_value( response, vfs );
        zmsg_pushmem( response, NULL, 0 );
    }

    // null id
    void populateResponseBad( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        RequestStatus requestStatus,
        RequestType requestType,
        const LHCVersionFlags& vfs )
    {
        frameHandler.prepend_message_value( response, requestStatus );
        zmsg_pushmem( response, NULL, 0 );
        frameHandler.prepend_message_value( response, requestType );
        frameHandler.prepend_message_value( response, vfs );
        zmsg_pushmem( response, NULL, 0 );
    }

    // null response
    void populateResponseBad( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        RequestId requestId,
        RequestType requestType,
        const LHCVersionFlags& vfs )
    {
        zmsg_pushmem( response, NULL, 0 );
        frameHandler.prepend_message_value( response, requestType );
        frameHandler.prepend_message_value( response, requestId );
        frameHandler.prepend_message_value( response, vfs );
        zmsg_pushmem( response, NULL, 0 );
    }

    // weird response
    void populateResponseBad( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        uint32_t badRequestStatus,
        RequestId requestId,
        RequestType requestType,
        const LHCVersionFlags& vfs )
    {
        frameHandler.prepend_message_value( response, badRequestStatus );
        frameHandler.prepend_message_value( response, requestType );
        frameHandler.prepend_message_value( response, requestId );
        frameHandler.prepend_message_value( response, vfs );
        zmsg_pushmem( response, NULL, 0 );
    }

    // no flags
    void populateResponseBad( ZMQMessage* response,
        ZMQFrameHandler& frameHandler,
        RequestStatus requestStatus,
        RequestId requestId,
        RequestType requestType )
    {
        frameHandler.prepend_message_value( response, requestStatus );
        frameHandler.prepend_message_value( response, requestType );
        frameHandler.prepend_message_value( response, requestId );
        zmsg_pushmem( response, NULL, 0 );
        zmsg_pushmem( response, NULL, 0 );
    }

    TEST_F( TestClient, TestFillResponse )
    {
        ZMQMessage* goodMessage = zmsg_new();
        ZMQMessage* goodMessagePlusData = zmsg_new();
        ZMQMessage* missingLeadingNull = zmsg_new();
        ZMQMessage* missingType = zmsg_new();
        ZMQMessage* missingId = zmsg_new();
        ZMQMessage* missingStatus = zmsg_new();
        ZMQMessage* badStatus = zmsg_new();
        // ZMQMessage* nullMessage = nullptr;
        ZMQMessage* missingVFS = zmsg_new();
        ZMQFrame* frame = nullptr;
        ZMQFrameHandler frameHandler;
        RequestResponse goodResponse;
        LHCVersionFlags vfs1( 999, 888, MessageFlag::Asynchronous );
        LHCVersionFlags vfs2( MessageFlag::AllowRedirect );
        LHCVersionFlags vfs3;

        populateResponseGood( goodMessage,
            frameHandler,
            RequestStatus::Failed,
            1,
            3,
            vfs1 );
        goodResponse.FillFromMessage( &goodMessage );
        ASSERT_EQ( 1, goodResponse.GetRequestId() );
        ASSERT_EQ( 3, goodResponse.GetRequestType() );
        ASSERT_EQ( RequestStatus::Failed, goodResponse.GetRequestStatus() );
        ASSERT_EQ( vfs1, goodResponse.GetVersionAndFlags() );
        ASSERT_EQ( 0, zmsg_size( goodResponse.GetMessageBody() ) );

        populateResponseGood( goodMessagePlusData,
            frameHandler,
            RequestStatus::Failed,
            5,
            13,
            vfs2 );
        frameHandler.append_message_value< RequestId >(
            goodMessagePlusData, 3 );
        RequestResponse goodResponsePlusData( &goodMessagePlusData );
        ASSERT_EQ( 5, goodResponsePlusData.GetRequestId() );
        ASSERT_EQ( 13, goodResponsePlusData.GetRequestType() );
        ASSERT_EQ( RequestStatus::Failed, goodResponsePlusData.GetRequestStatus() );
        ASSERT_EQ( vfs2, goodResponsePlusData.GetVersionAndFlags() );
        ASSERT_EQ( 1, zmsg_size( goodResponsePlusData.GetMessageBody() ) );

        populateResponseGood( missingLeadingNull,
            frameHandler,
            RequestStatus::Failed,
            1,
            1,
            vfs3 );
        frame = zmsg_pop( missingLeadingNull );
        zframe_destroy( &frame );
        EXPECT_EXCEPTION( BadRequestResponse,
            RequestResponse badResponse(
                &missingLeadingNull ); );
        zmsg_destroy( &missingLeadingNull );

        populateResponseBad( missingType,
            frameHandler,
            RequestStatus::Failed,
            (RequestId)1,
            vfs3 );
        EXPECT_EXCEPTION( BadRequestResponse,
            RequestResponse badResponse(
                &missingType ); );
        zmsg_destroy( &missingType );

        populateResponseBad( missingId,
            frameHandler,
            RequestStatus::Failed,
            (RequestType)1,
            vfs3 );
        EXPECT_EXCEPTION( BadRequestResponse,
            RequestResponse badResponse(
                &missingId ); );
        zmsg_destroy( &missingId );

        populateResponseBad( missingStatus,
            frameHandler,
            (RequestId)1,
            (RequestType)1,
            vfs3 );
        EXPECT_EXCEPTION( BadRequestResponse,
            RequestResponse badResponse(
                &missingStatus ); );
        zmsg_destroy( &missingStatus );

        populateResponseBad( missingVFS,
            frameHandler,
            RequestStatus::Failed,
            (RequestId)1,
            (RequestType)1 );
        EXPECT_EXCEPTION( BadRequestResponse,
            RequestResponse badResponse(
                &missingVFS ); );
        zmsg_destroy( &missingVFS );

        // TODO - does not quite throw, enhance the ZMQFraHandler
        //        or beware
        // populateResponseBad( badStatus,
        //                      frameHandler,
        //                      100000,
        //                      (RequestId) 1,
        //                      (RequestType) 1 );
        // EXPECT_EXCEPTION( BadRequestResponse,
        //                   RequestResponse badResponse(
        //                     badStatus ); );
        zmsg_destroy( &badStatus );

        // TODO - straight up crashes so maybe enhance
        //        RequestResponse to throw instead ?
        // EXPECT_EXCEPTION( BadRequestResponse,
        //                   RequestResponse badResponse(
        //                     nullMessage ); );
    }

    void checkClientMessage( ZMQMessage* msg,
        ZMQFrameHandler& frameHandler,
        MessageFlags messageFlags,
        RequestType requestType,
        RequestId requestId,
        std::vector< const char* > msgFrames )
    {
        LHCVersionFlags vfs( messageFlags );
        ASSERT_EQ( 5 + msgFrames.size(), zmsg_size( msg ) );

        ZMQFrame* frame = zmsg_first( msg );
        ASSERT_TRUE( zframe_size( frame ) == 0 );

        frame = zmsg_next( msg );
        ASSERT_EQ( ClusterMessage::ClientNewRequest,
            frameHandler.get_frame_value< ClusterMessage >( frame ) );

        frame = zmsg_next( msg );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( vfs,
            frameHandler.get_frame_value< LHCVersionFlags >( frame ) );

        frame = zmsg_next( msg );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( requestType,
            frameHandler.get_frame_value< RequestType >( frame ) );

        frame = zmsg_next( msg );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( requestId,
            frameHandler.get_frame_value< RequestId >( frame ) );

        for ( auto it = msgFrames.begin(); it != msgFrames.end(); ++it )
        {
            std::string data;
            frame = zmsg_next( msg );
            ASSERT_TRUE( zframe_size( frame ) > 0 );
            data = frameHandler.get_frame_value< std::string >( frame );
            ASSERT_EQ( std::string( *it ), data ) << *it << " != " << data.c_str();
        }
    }

    void checkClientMessage( ZMQMessage* msg,
        ZMQFrameHandler& frameHandler,
        MessageFlags messageFlags,
        RequestType requestType,
        RequestId requestId )
    {
        std::vector< const char* > data;
        checkClientMessage(
            msg, frameHandler, messageFlags, requestType, requestId, data );
    }

    TEST_F( TestClient, TestSendBasic )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointMessaging( EndpointType::InterProcess, "messaging" );
        Endpoint endpointProxy( EndpointType::InterProcess, "proxy" );
        TestZMQSocket* clientMessagingSocket = nullptr;
        ZMQFrameHandler frameHandler;
        MessageFlags messageFlags1( MessageFlag::ReceiptOnReceive );
        MessageFlags messageFlags2(
            MessageFlag::AllowRedirect | MessageFlag::RetryOnDeath );
        std::vector< const char* > data1{ "hello", "world" };
        std::vector< const char* > data2{ "wish", "you", "here" };
        std::vector< const char* > data3{ "data" };
        ZMQMessage* dataMessage1( zmsg_new() );
        ZMQMessage* dataMessage2( zmsg_new() );
        ZMQMessage* dataMessage3( zmsg_new() );

        populateFromDataFrames( frameHandler, dataMessage1, data1 );
        populateFromDataFrames( frameHandler, dataMessage2, data2 );
        populateFromDataFrames( frameHandler, dataMessage3, data3 );

        {
            std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            Client client( endpointMessaging,
                endpointProxy,
                std::move( lpSocketFactory ),
                std::move( lpSenderReceiver ) );

            clientMessagingSocket =
                (TestZMQSocket*)client.clientMessagingSocket;

            client.Activate();

            client.SendAsynchronousRequest( 3, 0 );
            client.SendSynchronousRequest( 4, 1 );
            client.SendRequest( 0, 5, messageFlags1 );

            client.SendAsynchronousRequest( 80, 9, &dataMessage1 );
            client.SendSynchronousRequest( 1, 6, &dataMessage2 );
            client.SendRequest( 81, 10, messageFlags2, &dataMessage3 );
        }

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 0 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 0 ].second,
            frameHandler,
            MessageFlag::Asynchronous,
            3, 0 ) );

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 1 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 1 ].second,
            frameHandler,
            MessageFlag::None,
            4, 1 ) );

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 2 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 2 ].second,
            frameHandler,
            messageFlags1,
            0, 5 ) );

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 3 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 3 ].second,
            frameHandler,
            MessageFlag::Asynchronous,
            80, 9, data1 ) );

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 4 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 4 ].second,
            frameHandler,
            MessageFlag::None,
            1, 6, data2 ) );

        ASSERT_EQ( clientMessagingSocket, senderReceiver.sentMessages[ 5 ].first );
        ASSERT_NO_FATAL_FAILURE( checkClientMessage(
            senderReceiver.sentMessages[ 5 ].second,
            frameHandler,
            messageFlags2,
            81, 10, data3 ) );
    }

    // TODO - allow poller to be mocked out so receive can be tested
}
