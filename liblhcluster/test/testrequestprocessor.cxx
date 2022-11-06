#include <lhcluster_impl/requestprocessor.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "testzmqsocketutils.h"
#include "test.h"

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    class TestRequestProcessor : public ::testing::Test
    {
    protected:

        void SetUp()
        {

        }

        void Tear()
        {
        }
    };

    TEST_F( TestRequestProcessor, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfMessagingEndpoint( EndpointType::TCP, "selfMessaging" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfController" );
        TestZMQSocket* selfMessaging = nullptr;
        TestZMQSocket* selfSecController = nullptr;
        TestZMQSocket* selfPrimController = nullptr;

        {
            std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            std::unique_ptr< DummyRequestHandler > lpDummyRequestHandler(
                new DummyRequestHandler() );
            // DummyRequestHandler* dummyRequestHandler = lpDummyRequestHandler.get();
            RequestProcessor dummyRequestProcessor(
                selfMessagingEndpoint,
                selfControllerEndpoint,
                move( lpSocketFactory ),
                move( lpSenderReceiver ),
                move( lpDummyRequestHandler ) );

            ASSERT_EQ( selfMessagingEndpoint,
                dummyRequestProcessor.selfMessagingEndpoint );
            ASSERT_EQ( selfControllerEndpoint,
                dummyRequestProcessor.selfControllerEndpoint );

            selfSecController = (TestZMQSocket*)dummyRequestProcessor.selfSecondarySocket;
            selfPrimController = (TestZMQSocket*)dummyRequestProcessor.selfPrimarySocket;
            dummyRequestProcessor.activateControllerSockets();
            ASSERT_EQ( SocketType::Pair, selfSecController->socketType );
            ASSERT_EQ( SocketType::Pair, selfPrimController->socketType );
            ASSERT_FALSE( selfSecController->destroyed );
            ASSERT_FALSE( selfPrimController->destroyed );
            ASSERT_EQ( 1, selfSecController->binded.size() );
            ASSERT_EQ( 0, selfSecController->connected.size() );
            ASSERT_EQ( 0, selfSecController->unBinded.size() );
            ASSERT_TRUE( selfSecController->binded.find(
                selfControllerEndpoint.path() ) != selfSecController->binded.end() );
            ASSERT_EQ( 1, selfPrimController->connected.size() );
            ASSERT_EQ( 0, selfPrimController->binded.size() );
            ASSERT_EQ( 0, selfPrimController->disConnected.size() );
            ASSERT_TRUE( selfPrimController->connected.find(
                selfControllerEndpoint.path() ) != selfPrimController->connected.end() );

            selfMessaging = (TestZMQSocket*)dummyRequestProcessor.selfMessagingSocket;
            dummyRequestProcessor.activateMessagingSockets();
            ASSERT_EQ( SocketType::Pair, selfMessaging->socketType );
            ASSERT_FALSE( selfMessaging->destroyed );
            ASSERT_EQ( 1, selfMessaging->binded.size() );
            ASSERT_EQ( 0, selfMessaging->connected.size() );
            ASSERT_EQ( 0, selfMessaging->unBinded.size() );
            ASSERT_EQ( 0, selfMessaging->disConnected.size() );
            ASSERT_TRUE( selfMessaging->binded.find(
                selfMessagingEndpoint.path() ) != selfMessaging->binded.end() );

            // called when startAs... exits
            dummyRequestProcessor.deactivateMessagingSockets();
            ASSERT_TRUE( selfMessaging->destroyed );
            // should be refreshed
            ASSERT_FALSE( selfMessaging ==
                (TestZMQSocket*)dummyRequestProcessor.selfMessagingSocket );
            ASSERT_EQ( 1, selfMessaging->binded.size() );
            ASSERT_EQ( 0, selfMessaging->connected.size() );
            ASSERT_EQ( 1, selfMessaging->unBinded.size() );
            ASSERT_EQ( 0, selfMessaging->disConnected.size() );
            ASSERT_TRUE( selfMessaging->unBinded.find(
                selfMessagingEndpoint.path() ) != selfMessaging->unBinded.end() );
            selfMessaging = (TestZMQSocket*)dummyRequestProcessor.selfMessagingSocket;
        }

        ASSERT_TRUE( selfSecController->destroyed );
        ASSERT_TRUE( selfPrimController->destroyed );
        ASSERT_EQ( 1, selfSecController->binded.size() );
        ASSERT_EQ( 0, selfSecController->connected.size() );
        ASSERT_EQ( 1, selfSecController->unBinded.size() );
        ASSERT_EQ( 1, selfPrimController->connected.size() );
        ASSERT_EQ( 0, selfPrimController->binded.size() );
        ASSERT_EQ( 1, selfPrimController->disConnected.size() );

        ASSERT_TRUE( selfMessaging->destroyed );
        ASSERT_EQ( 0, selfMessaging->binded.size() );
        ASSERT_EQ( 0, selfMessaging->connected.size() );
        ASSERT_EQ( 0, selfMessaging->unBinded.size() );
        ASSERT_EQ( 0, selfMessaging->disConnected.size() );
    }


    // No stats so must test return values
    TEST_F( TestRequestProcessor, TestHandleRequest )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfMessagingEndpoint( EndpointType::TCP, "selfMessaging" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfController" );
        ZMQFrameHandler frameHandler;
        ZMQMessage* unsentGoodMessage = nullptr;
        ZMQMessage* missingRequestType = nullptr;
        ZMQMessage* missingRequestId = nullptr;
        ZMQMessage* goodMessage = nullptr;
        ZMQMessage* goodMessagePlus = nullptr;
        ZMQMessage* missingVFS = nullptr;
        LHCVersionFlags vfs1( MessageFlag::Asynchronous | MessageFlag::AllowRedirect );
        LHCVersionFlags vfs2( 999, 3, MessageFlag::NotifyOnRedirect );
        LHCVersionFlags vfs3;
        RequestType requestType1 = 130;
        RequestType requestType2 = 2300;
        RequestType requestType3 = 330009;
        RequestId requestId1 = 8100;
        RequestId requestId2 = 8200;
        RequestId requestId3 = 8300;

        //cases
        // receive fails
        // send fails
        // request type missing
        // requst id missing
        // good message with no data
        // good message with data
        // vfs missing
        unsentGoodMessage = createRequestMessage(
            frameHandler, &vfs1, &requestType1, &requestId1 );
        missingRequestType = createRequestMessage(
            frameHandler, &vfs1, nullptr, &requestId1 );
        missingRequestId = createRequestMessage(
            frameHandler, &vfs1, &requestType1, nullptr );
        goodMessage = createRequestMessage(
            frameHandler, &vfs2, &requestType2, &requestId2 );
        goodMessagePlus = createRequestMessage(
            frameHandler, &vfs3, &requestType3, &requestId3 );
        missingVFS = createRequestMessage(
            frameHandler, nullptr, &requestType1, &requestId1 );
        frameHandler.append_message_value< string >( goodMessagePlus, "data" );

        senderReceiver.receiveReturnValues = {
            nullptr,
            unsentGoodMessage,
            missingRequestType,
            missingRequestId,
            goodMessage,
            goodMessagePlus,
            missingVFS
        };

        senderReceiver.sendReturnValues = {
            853,
            0
        };

        {
            std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            std::unique_ptr< DummyRequestHandler > lpDummyRequestHandler(
                new DummyRequestHandler() );
            DummyRequestHandler* dummyRequestHandler = lpDummyRequestHandler.get();
            RequestProcessor dummyRequestProcessor(
                selfMessagingEndpoint,
                selfControllerEndpoint,
                move( lpSocketFactory ),
                move( lpSenderReceiver ),
                move( lpDummyRequestHandler ) );
            int ret = 0;
            ZMQMessage* nullFrameMessage = nullptr;

            nullFrameMessage = zmsg_new();
            zmsg_pushmem( nullFrameMessage, nullptr, 0 );

            // return empty msg, nullmsg, big message
            dummyRequestHandler->processRetValues = {
                { RequestStatus::Succeeded,
                  nullFrameMessage },
                { RequestStatus::Failed,
                  zmsg_new() },
                { RequestStatus::Succeeded,
                  zmsg_dup( goodMessagePlus ) }
            };

            // fail to receive
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 1, ret );
            ASSERT_EQ( 1, senderReceiver.receiveCount );
            ASSERT_EQ( 0, senderReceiver.sendCount );

            // fail to send
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 2, senderReceiver.receiveCount );
            ASSERT_EQ( 1, senderReceiver.sendCount );
            ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 1, dummyRequestHandler->numRequestsProcessed );

            // fail to parse request type
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 3, senderReceiver.receiveCount );
            ASSERT_EQ( 1, senderReceiver.sendCount );
            ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 1, dummyRequestHandler->numRequestsProcessed );

            // fail to parse reques id
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 4, senderReceiver.receiveCount );
            ASSERT_EQ( 1, senderReceiver.sendCount );
            ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 1, dummyRequestHandler->numRequestsProcessed );

            // good message, i think this will crash if the response msg is null
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 5, senderReceiver.receiveCount );
            ASSERT_EQ( 2, senderReceiver.sendCount );
            ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 2, dummyRequestHandler->numRequestsProcessed );
            ASSERT_EQ( 4,
                zmsg_size(
                    senderReceiver.sentMessages[ 1 ].second ) );
            ASSERT_EQ( (TestZMQSocket*)dummyRequestProcessor.selfMessagingSocket,
                senderReceiver.sentMessages[ 1 ].first );
            ASSERT_EQ( vfs2,
                frameHandler.get_frame_value< LHCVersionFlags >(
                    zmsg_first( senderReceiver.sentMessages[ 1 ].second ) ) );
            ASSERT_EQ( requestType2,
                frameHandler.get_frame_value< RequestType >(
                    zmsg_next( senderReceiver.sentMessages[ 1 ].second ) ) );
            ASSERT_EQ( requestId2,
                frameHandler.get_frame_value< RequestId >(
                    zmsg_next( senderReceiver.sentMessages[ 1 ].second ) ) );
            ASSERT_EQ( RequestStatus::Failed,
                frameHandler.get_frame_value< RequestStatus >(
                    zmsg_next( senderReceiver.sentMessages[ 1 ].second ) ) );

            // good message with data
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 6, senderReceiver.receiveCount );
            ASSERT_EQ( 3, senderReceiver.sendCount );
            ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 3, dummyRequestHandler->numRequestsProcessed );
            ASSERT_EQ( 4 + zmsg_size( goodMessagePlus ),
                zmsg_size(
                    senderReceiver.sentMessages[ 2 ].second ) );
            ASSERT_EQ( (TestZMQSocket*)dummyRequestProcessor.selfMessagingSocket,
                senderReceiver.sentMessages[ 2 ].first );
            ASSERT_EQ( vfs3,
                frameHandler.get_frame_value< LHCVersionFlags >(
                    zmsg_first( senderReceiver.sentMessages[ 2 ].second ) ) );
            ASSERT_EQ( requestType3,
                frameHandler.get_frame_value< RequestType >(
                    zmsg_next( senderReceiver.sentMessages[ 2 ].second ) ) );
            ASSERT_EQ( requestId3,
                frameHandler.get_frame_value< RequestId >(
                    zmsg_next( senderReceiver.sentMessages[ 2 ].second ) ) );
            ASSERT_EQ( RequestStatus::Succeeded,
                frameHandler.get_frame_value< RequestStatus >(
                    zmsg_next( senderReceiver.sentMessages[ 2 ].second ) ) );
            ASSERT_EQ( "data",
                frameHandler.get_frame_value< string >(
                    zmsg_last( senderReceiver.sentMessages[ 2 ].second ) ) );

            // missing vfs
            ret = dummyRequestProcessor.handleRequest();
            ASSERT_EQ( 0, ret );
            ASSERT_EQ( 7, senderReceiver.receiveCount );
            ASSERT_EQ( 3, senderReceiver.sendCount );
            ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
            ASSERT_EQ( 3, dummyRequestHandler->numRequestsProcessed );
        }
    }
}
