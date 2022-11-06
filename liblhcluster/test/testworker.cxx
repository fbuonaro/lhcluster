#include <lhcluster_impl/worker.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "testzmqsocketutils.h"
#include "test.h"

#include <gtest/gtest.h>
#include <vector>
#include <string>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    class TestWorker : public ::testing::Test
    {
    };

    void checkRTCAndIPAndRC(
        int lineNumber,
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        bool hasRTContext,
        bool isInProgress,
        bool hasRContext )
    {
        auto it = worker.requestTypeContexts.find( requestType );
        auto it2 = worker.requestContexts.find(
            pair< RequestType, RequestId >( requestType, requestId ) );

        if ( hasRTContext )
        {
            ASSERT_TRUE( it != worker.requestTypeContexts.end() ) << "failed at " << lineNumber;
            if ( isInProgress )
            {
                ASSERT_TRUE(
                    it->second.GetInProgressRequests().find( requestId ) !=
                    it->second.GetInProgressRequests().end() ) << "failed at " << lineNumber;
            }
            else
            {
                ASSERT_TRUE(
                    it->second.GetInProgressRequests().find( requestId ) ==
                    it->second.GetInProgressRequests().end() ) << "failed at " << lineNumber;
            }
        }
        else
        {
            ASSERT_TRUE( it == worker.requestTypeContexts.end() ) << "failed at " << lineNumber;
        }

        if ( hasRContext )
        {
            ASSERT_TRUE( it2 != worker.requestContexts.end() ) << "failed at " << lineNumber;
        }
        else
        {
            ASSERT_TRUE( it2 == worker.requestContexts.end() ) << "failed at " << lineNumber;
        }
    }

    void checkRequestTypeContextInProgress(
        Worker& worker,
        RequestType requestType,
        RequestId requestId )
    {
        auto it = worker.requestTypeContexts.find( requestType );
        ASSERT_TRUE( it != worker.requestTypeContexts.end() );
        ASSERT_TRUE(
            it->second.GetInProgressRequests().find( requestId ) !=
            it->second.GetInProgressRequests().end() );
    }

    void checkRequestTypeContext(
        Worker& worker,
        RequestType requestType,
        unsigned int numInProgressRequests )
    {
        auto it = worker.requestTypeContexts.find( requestType );
        ASSERT_TRUE( it != worker.requestTypeContexts.end() );
        ASSERT_TRUE( requestType == it->second.GetRequestType() );
        ASSERT_TRUE( numInProgressRequests ==
            it->second.GetInProgressRequests().size() );
    }

    void checkRequestTypeContext(
        Worker& worker,
        RequestType requestType )
    {
        return checkRequestTypeContext( worker, requestType, 0 );
    }

    unique_ptr< Worker >
        createWorker(
            TestZMQSocketFactory& socketFactory,
            TestZMQSocketSenderReceiver& senderReceiver,
            const vector< RequestType >& supportedRequestTypes )
    {
        Endpoint brokerMessagingEndpoint( EndpointType::TCP, "brokerMessaging" );
        Endpoint selfMessagingEndpoint( EndpointType::TCP, "selfMessaging" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfController" );
        Endpoint rpMessagingEndpoint( EndpointType::TCP, "rpMessaging" );
        Endpoint rpControllerEndpoint( EndpointType::TCP, "rpController" );
        Delay selfHeartbeatDelay( 30 );
        Delay initBrokerHeartbeatDelay( 60 );
        Delay brokerPulseDelay( 120 );
        Delay expectedBrokerDeathDelay( 2 * selfHeartbeatDelay );
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        std::unique_ptr< IZMQSocketFactory > lpRPSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpRPSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        std::unique_ptr< DummyRequestHandler > lpDummyRequestHandler(
            new DummyRequestHandler() );
        DummyRequestHandler* dummyRequestHandler = lpDummyRequestHandler.get();
        dummyRequestHandler->supportedRequestTypes = supportedRequestTypes;

        return unique_ptr< Worker>( new Worker(
            selfMessagingEndpoint,
            brokerMessagingEndpoint,
            selfControllerEndpoint,
            selfHeartbeatDelay,
            initBrokerHeartbeatDelay,
            brokerPulseDelay,
            move( lpSocketFactory ),
            move( lpSenderReceiver ),
            rpMessagingEndpoint,
            rpControllerEndpoint,
            move( lpDummyRequestHandler ),
            move( lpRPSocketFactory ),
            move( lpRPSenderReceiver ) ) );
    }

    TEST_F( TestWorker, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfMessagingEndpoint( EndpointType::TCP, "selfMessaging" );
        Endpoint brokerMessagingEndpoint( EndpointType::TCP, "brokerMessaging" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfController" );
        TestZMQSocket* messagingSocket = nullptr;
        TestZMQSocket* listenerSocket = nullptr;
        TestZMQSocket* signalerSocket = nullptr;
        Delay selfHeartbeatDelay( 30 );
        Delay initBrokerHeartbeatDelay( 60 );
        Delay brokerPulseDelay( 120 );
        Delay expectedBrokerDeathDelay( 2 * selfHeartbeatDelay );
        bool thrown = false;

        // try no request types
        try
        {
            std::unique_ptr< Worker > worker(
                createWorker( socketFactory, senderReceiver, {} ) );
        }
        catch ( const WorkerConstructionFailed& e )
        {
            thrown = true;
        }
        catch ( ... )
        {
            ASSERT_TRUE( false ) << "unexpected constructor exception";
        }

        ASSERT_TRUE( thrown ) << "no exception thrown for no request types";

        {
            std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            Endpoint rpMessagingEndpoint( EndpointType::TCP, "rpMessaging" );
            Endpoint rpControllerEndpoint( EndpointType::TCP, "rpController" );
            std::unique_ptr< IZMQSocketFactory > lpRPSocketFactory(
                new TestZMQSocketFactoryProxy( socketFactory ) );
            std::unique_ptr< IZMQSocketSenderReceiver > lpRPSenderReceiver(
                new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
            std::unique_ptr< DummyRequestHandler > lpDummyRequestHandler(
                new DummyRequestHandler() );
            DummyRequestHandler* dummyRequestHandler = lpDummyRequestHandler.get();
            dummyRequestHandler->supportedRequestTypes = { 1,2,3,4,5 };
            Worker worker(
                selfMessagingEndpoint,
                brokerMessagingEndpoint,
                selfControllerEndpoint,
                selfHeartbeatDelay,
                initBrokerHeartbeatDelay,
                brokerPulseDelay,
                move( lpSocketFactory ),
                move( lpSenderReceiver ),
                rpMessagingEndpoint,
                rpControllerEndpoint,
                move( lpDummyRequestHandler ),
                move( lpRPSocketFactory ),
                move( lpRPSenderReceiver ) );

            messagingSocket = (TestZMQSocket*)worker.messagingSocket;
            listenerSocket = (TestZMQSocket*)worker.listenerSocket;
            signalerSocket = (TestZMQSocket*)worker.signalerSocket;

            ASSERT_EQ( initBrokerHeartbeatDelay,
                worker.brokerHeartbeatDelay );
            ASSERT_EQ( selfHeartbeatDelay, worker.selfHeartbeatDelay );
            ASSERT_EQ( brokerPulseDelay, worker.brokerPulseDelay );
            ASSERT_EQ( expectedBrokerDeathDelay, worker.brokerDeathDelay );

            worker.activateControllerSockets();
            ASSERT_EQ( 1, listenerSocket->binded.size() );
            ASSERT_EQ( 0, listenerSocket->connected.size() );
            ASSERT_TRUE(
                listenerSocket->binded.find(
                    selfControllerEndpoint.path() ) !=
                listenerSocket->binded.end() );
            ASSERT_EQ( 0, signalerSocket->binded.size() );
            ASSERT_EQ( 1, signalerSocket->connected.size() );
            ASSERT_TRUE(
                signalerSocket->connected.find(
                    selfControllerEndpoint.path() ) !=
                signalerSocket->connected.end() );

            worker.activateMessagingSockets();
            ASSERT_EQ( 1, messagingSocket->binded.size() );
            ASSERT_EQ( 1, messagingSocket->connected.size() );
            ASSERT_TRUE(
                messagingSocket->binded.find(
                    selfMessagingEndpoint.path() ) !=
                messagingSocket->binded.end() );
            ASSERT_TRUE(
                messagingSocket->connected.find(
                    brokerMessagingEndpoint.path() ) !=
                messagingSocket->connected.end() );

            worker.deactivateMessagingSockets();
            ASSERT_FALSE( messagingSocket->destroyed );
            ASSERT_EQ( 1, messagingSocket->unBinded.size() );
            ASSERT_EQ( 1, messagingSocket->disConnected.size() );
            ASSERT_EQ( 1, messagingSocket->binded.size() );
            ASSERT_EQ( 1, messagingSocket->connected.size() );
            ASSERT_TRUE(
                messagingSocket->unBinded.find(
                    selfMessagingEndpoint.path() ) !=
                messagingSocket->unBinded.end() );
            ASSERT_TRUE(
                messagingSocket->disConnected.find(
                    brokerMessagingEndpoint.path() ) !=
                messagingSocket->disConnected.end() );

            ASSERT_EQ( 5, worker.requestTypeContexts.size() );

            ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( worker, 1 ) );
            ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( worker, 2 ) );
            ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( worker, 3 ) );
            ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( worker, 4 ) );
            ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( worker, 5 ) );
        }

        ASSERT_TRUE( messagingSocket->destroyed );
        ASSERT_EQ( 1, messagingSocket->unBinded.size() );
        ASSERT_EQ( 1, messagingSocket->disConnected.size() );
        ASSERT_EQ( 1, messagingSocket->binded.size() );
        ASSERT_EQ( 1, messagingSocket->connected.size() );

        ASSERT_TRUE( listenerSocket->destroyed );
        ASSERT_TRUE( signalerSocket->destroyed );
        ASSERT_EQ( 1, listenerSocket->binded.size() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );
        ASSERT_EQ( 1, listenerSocket->unBinded.size() );
        ASSERT_EQ( 0, listenerSocket->disConnected.size() );
        ASSERT_TRUE(
            listenerSocket->unBinded.find(
                selfControllerEndpoint.path() ) !=
            listenerSocket->unBinded.end() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        ASSERT_EQ( 1, signalerSocket->connected.size() );
        ASSERT_EQ( 0, signalerSocket->unBinded.size() );
        ASSERT_EQ( 1, signalerSocket->disConnected.size() );
        ASSERT_TRUE(
            signalerSocket->disConnected.find(
                selfControllerEndpoint.path() ) !=
            signalerSocket->disConnected.end() );
    }

    TEST_F( TestWorker, TestAddNewInProgressRequest )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 3,53,853,9853,1 } ) );

        worker->addNewInProgressRequest( 3, 555 );
        worker->addNewInProgressRequest( 3, 556 );
        worker->addNewInProgressRequest( 3, 557 );
        worker->addNewInProgressRequest( 3, 558 );
        worker->addNewInProgressRequest( 3, 559 );

        worker->addNewInProgressRequest( 53, 1000 );
        worker->addNewInProgressRequest( 853, 1000 );
        worker->addNewInProgressRequest( 9853, 9000 );
        worker->addNewInProgressRequest( 1, 1 );

        worker->addNewInProgressRequest( 2, 1000 );
        worker->addNewInProgressRequest( 4, 1000 );
        worker->addNewInProgressRequest( 5, 1000 );

        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( *worker, 3, 5 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( *worker, 53, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( *worker, 853, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( *worker, 9853, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContext( *worker, 1, 1 ) );

        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 3, 555 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 3, 556 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 3, 557 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 3, 558 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 3, 559 ) );

        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 53, 1000 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 853, 1000 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 9853, 9000 ) );
        ASSERT_NO_FATAL_FAILURE( checkRequestTypeContextInProgress( *worker, 1, 1 ) );
    }

    TEST_F( TestWorker, TestIsBrokerAliveAsOf )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 3 } ) );

        worker->brokerLastActive = 10;
        worker->brokerDeathDelay = Delay( 26 );
        ASSERT_TRUE( worker->isBrokerAliveAsOf( 20 ) );
        ASSERT_TRUE( worker->isBrokerAliveAsOf( 1 ) );
        ASSERT_TRUE( worker->isBrokerAliveAsOf( 35 ) );
        ASSERT_TRUE( worker->isBrokerAliveAsOf( 36 ) );
        ASSERT_FALSE( worker->isBrokerAliveAsOf( 37 ) );
        ASSERT_FALSE( worker->isBrokerAliveAsOf( 37000000 ) );
    }

    void checkClientRequestContext(
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        vector< ZMQFrame* >& envelope
    )
    {
        auto it = worker.requestContexts.find(
            pair< RequestType, RequestId >( requestType, requestId ) );
        ASSERT_TRUE( it != worker.requestContexts.end() );
        ASSERT_EQ( requestType, it->second.GetRequestType() );
        ASSERT_EQ( requestId, it->second.GetRequestId() );
        ASSERT_EQ( vfs, it->second.GetVersionMessageFlags() );
        ASSERT_EQ( envelope.size(), it->second.GetEnvelope().size() );
        for ( auto it2 = envelope.begin(); it2 != envelope.end(); ++it2 )
        {
            ASSERT_TRUE(
                zframe_eq(
                    *it2,
                    it->second.GetEnvelope()[ it2 - envelope.begin() ] ) );
        }
    }


    TEST_F( TestWorker, TestRequestRedirected )
    {
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        vector< int > ints{ 1, 2, 3, 4, 5, 6 };
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 3 } ) );
        vector< ZMQFrame* > env1{
            zframe_new( ints.data(), sizeof( int ) ) };
        vector< ZMQFrame* > env2{
            zframe_new( ints.data() + 1, sizeof( int ) ),
            zframe_new( ints.data() + 2, sizeof( int ) ) };
        vector< ZMQFrame* > env3{
            zframe_new( ints.data() + 3, sizeof( int ) ),
            zframe_new( ints.data() + 4, sizeof( int ) ),
            zframe_new( ints.data() + 5, sizeof( int ) ) };
        ClientRequestContext crtc1(
            1, 1, MessageFlag::None,
            zmqFrameVectorCopy( env3 ) );
        ClientRequestContext crtc2(
            20, 20, MessageFlag::None,
            zmqFrameVectorCopy( env1 ) );
        ClientRequestContext crtc3(
            300, 300, MessageFlag::None,
            zmqFrameVectorCopy( env2 ) );
        LHCVersionFlags vfs( MessageFlag::None );

        addClientRequestContext(
            *worker,
            1, 1, vfs,
            zmqFrameVectorCopy( env1 ) );
        addClientRequestContext(
            *worker,
            20, 20, vfs,
            zmqFrameVectorCopy( env2 ) );
        addClientRequestContext(
            *worker,
            300, 300, vfs,
            zmqFrameVectorCopy( env3 ) );

        ASSERT_EQ( 3, worker->requestContexts.size() );
        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            1, 1, vfs,
            env1 ) );
        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            20, 20, vfs,
            env2 ) );
        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            300, 300, vfs,
            env3 ) );

        ASSERT_TRUE( worker->requestRedirected( crtc1 ) );
        ASSERT_TRUE( worker->requestRedirected( crtc2 ) );
        ASSERT_TRUE( worker->requestRedirected( crtc3 ) );

        ASSERT_EQ( 0, crtc1.GetEnvelope().size() );
        ASSERT_EQ( 0, crtc2.GetEnvelope().size() );
        ASSERT_EQ( 0, crtc3.GetEnvelope().size() );

        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            1, 1, vfs,
            env3 ) );
        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            20, 20, vfs,
            env1 ) );
        ASSERT_NO_FATAL_FAILURE( checkClientRequestContext(
            *worker,
            300, 300, vfs,
            env2 ) );
    }

    TEST_F( TestWorker, TestHeartbeatNeededAsOf )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 3 } ) );
        // Defaults from createWorker
        // Delay selfHeartbeatDelay( 30 );
        // Delay initBrokerHeartbeatDelay( 60 );
        // Delay brokerPulseDelay( 120 );
        // Delay expectedBrokerDeathDelay( 2 * selfHeartbeatDelay );

        worker->workerLastActive = 10;
        ASSERT_TRUE( worker->heartbeatNeededAsOf( 70 ) );
        ASSERT_TRUE( worker->heartbeatNeededAsOf( 71 ) );
        ASSERT_TRUE( worker->heartbeatNeededAsOf( 100 ) );
        ASSERT_FALSE( worker->heartbeatNeededAsOf( 19 ) );
        ASSERT_FALSE( worker->heartbeatNeededAsOf( 1 ) );
        ASSERT_FALSE( worker->heartbeatNeededAsOf( 69 ) );

    }

    TEST_F( TestWorker, TestSendBrokerReadyMessage )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory1;
        TestZMQSocketSenderReceiver senderReceiver1;
        TestZMQSocketFactory socketFactory2;
        TestZMQSocketSenderReceiver senderReceiver2;
        unique_ptr< Worker > worker1(
            createWorker( socketFactory1, senderReceiver1, { 10000 } ) );
        unique_ptr< Worker > worker2(
            createWorker( socketFactory2, senderReceiver2, { 10, 200, 3000 } ) );
        LHCVersionFlags vfs;

        // A Broker Ready message consists of
        //  1.  null frame
        //  2.  type ready
        //  3.  unsigned int free capacitty
        //  4.  heartbeat delay
        //  5.  one or more request types

        // It will
        //  1.   update last active on send success
        //  2.  on send success, increment readiessent
        //  3.  on send fail, increment readiessentfailed

        // Test cases
        //  0.  send fails - fail stats inc'd, no update last active
        //  1.  Send with one supported rtpe - succ stats incd and update last active
        //  2.  Send with four supported rtpes - succ stats incd  and update last active
        //  3.  Send with four supported request types
        //      with some requests in progress - succ stats incd and update last active

        worker1->currentTime = 3;
        senderReceiver1.sendReturnValues = { 1, 0 };

        // worker1 will handle cases 0 and 1
        ret = worker1->sendBrokerReadyMessage();
        ASSERT_EQ( 1, ret );
        // last active should be 0 still, ReadiesSentFailed should be 1
        ASSERT_EQ( 0, worker1->workerLastActive );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::ReadiesSentFailed ] );
        ASSERT_EQ( 1, senderReceiver1.sendCount );
        ASSERT_EQ( nullptr, senderReceiver1.sentMessages[ 0 ].second );
        ASSERT_EQ( (TestZMQSocket*)worker1->messagingSocket,
            senderReceiver1.sentMessages[ 0 ].first );

        ret = worker1->sendBrokerReadyMessage();
        ASSERT_EQ( 0, ret );
        // last active should be 3 now, ReadiesSentFailed should be 1, ReadiesSent 1
        ASSERT_EQ( 3, worker1->workerLastActive );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::ReadiesSent ] );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::ReadiesSentFailed ] );
        ASSERT_EQ( 2, senderReceiver1.sendCount );
        ASSERT_FALSE( nullptr == senderReceiver1.sentMessages[ 1 ].second );
        ASSERT_EQ( 6, zmsg_size( senderReceiver1.sentMessages[ 1 ].second ) );
        ASSERT_EQ( (TestZMQSocket*)worker1->messagingSocket,
            senderReceiver1.sentMessages[ 1 ].first );
        ASSERT_EQ(
            0,
            zframe_size( zmsg_first( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            ClusterMessage::WorkerReady,
            frameHandler.get_frame_value< ClusterMessage >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            vfs,
            frameHandler.get_frame_value< LHCVersionFlags >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            worker1->capacity,
            frameHandler.get_frame_value< unsigned int >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            worker1->selfHeartbeatDelay.count(),
            frameHandler.get_frame_value< Delay::rep >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            10000,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );

        worker2->currentTime = 5;
        worker2->capacity = 3;
        ret = worker2->sendBrokerReadyMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 5, worker2->workerLastActive );
        STATS_ASSERT_EQ( 1, worker2->stats[ WorkerStat::ReadiesSent ] );
        STATS_ASSERT_EQ( 0, worker2->stats[ WorkerStat::ReadiesSentFailed ] );
        ASSERT_EQ( 1, senderReceiver2.sendCount );
        ASSERT_FALSE( nullptr == senderReceiver2.sentMessages[ 0 ].second );
        // 10,200,3000
        ASSERT_EQ( 8, zmsg_size( senderReceiver2.sentMessages[ 0 ].second ) );
        ASSERT_EQ( (TestZMQSocket*)worker2->messagingSocket,
            senderReceiver2.sentMessages[ 0 ].first );
        ASSERT_EQ(
            0,
            zframe_size( zmsg_first( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            ClusterMessage::WorkerReady,
            frameHandler.get_frame_value< ClusterMessage >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            vfs,
            frameHandler.get_frame_value< LHCVersionFlags >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            worker2->capacity,  // 3
            frameHandler.get_frame_value< unsigned int >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            worker2->selfHeartbeatDelay.count(),
            frameHandler.get_frame_value< Delay::rep >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            10,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            200,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );
        ASSERT_EQ(
            3000,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 0 ].second ) ) );

        worker2->currentTime = 9;
        worker2->addNewInProgressRequest( 10, 1 );
        worker2->addNewInProgressRequest( 3000, 1 );
        ret = worker2->sendBrokerReadyMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, worker2->workerLastActive );
        STATS_ASSERT_EQ( 2, worker2->stats[ WorkerStat::ReadiesSent ] );
        STATS_ASSERT_EQ( 0, worker2->stats[ WorkerStat::ReadiesSentFailed ] );
        ASSERT_EQ( 2, senderReceiver2.sendCount );
        ASSERT_FALSE( nullptr == senderReceiver2.sentMessages[ 1 ].second );
        // 10,200,3000
        ASSERT_EQ( 8, zmsg_size( senderReceiver2.sentMessages[ 1 ].second ) );
        ASSERT_EQ( (TestZMQSocket*)worker2->messagingSocket,
            senderReceiver2.sentMessages[ 1 ].first );
        ASSERT_EQ(
            0,
            zframe_size( zmsg_first( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            ClusterMessage::WorkerReady,
            frameHandler.get_frame_value< ClusterMessage >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            vfs,
            frameHandler.get_frame_value< LHCVersionFlags >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            1,  // 3 - 2, added two in progress requests
            frameHandler.get_frame_value< unsigned int >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            worker2->selfHeartbeatDelay.count(),
            frameHandler.get_frame_value< Delay::rep >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            10,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            200,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            3000,
            frameHandler.get_frame_value< RequestType >(
                zmsg_next( senderReceiver2.sentMessages[ 1 ].second ) ) );
    }

    TEST_F( TestWorker, TestSendBrokerHeartbeatMessage )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory1;
        TestZMQSocketSenderReceiver senderReceiver1;
        unique_ptr< Worker > worker1(
            createWorker( socketFactory1, senderReceiver1, { 10000 } ) );

        // should always return 0
        // do nothing if heartbeat not needed as of current time
        // populate a heartbeat message as per SendHeartbeat
        // 1, null frame
        // 2. header value ClusterMessage::WorkerHeartbeat
        // 3. sentAsOfTime (currentTime)
        // 4. delay - selfHeartbeatDelay
        // On success - ++WorkerStat::BrokerHeartbeatsSent
        // On fail - ++WorkerStat::BrokerHeartbeatsSentFailed

        // three cases
        // 1.   heartbeat not needed (last active+broker hb delay > current time)
        //      -> nothing sent
        // 2.   send failed - inc stat
        // 3.   send successful - formatted as expected

        senderReceiver1.sendReturnValues = { 1, 0 };

        worker1->workerLastActive = 5;
        worker1->currentTime = 10;
        worker1->brokerHeartbeatDelay = Delay( 6 );
        // 5 + 6 > 10 -> no hb needed
        ret = worker1->sendBrokerHeartbeat();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 0, worker1->stats[ WorkerStat::BrokerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 0, worker1->stats[ WorkerStat::BrokerHeartbeatsSentFailed ] );
        ASSERT_EQ( 0, senderReceiver1.sendCount );

        worker1->workerLastActive = 4; // 4 + 6 == 10 -> need heartbeat
        ret = worker1->sendBrokerHeartbeat();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 0, worker1->stats[ WorkerStat::BrokerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::BrokerHeartbeatsSentFailed ] );
        ASSERT_EQ( 1, senderReceiver1.sendCount );
        ASSERT_EQ( (TestZMQSocket*)worker1->messagingSocket,
            senderReceiver1.sentMessages[ 0 ].first );

        ret = worker1->sendBrokerHeartbeat();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::BrokerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, worker1->stats[ WorkerStat::BrokerHeartbeatsSentFailed ] );
        ASSERT_EQ( 2, senderReceiver1.sendCount );
        ASSERT_EQ( (TestZMQSocket*)worker1->messagingSocket,
            senderReceiver1.sentMessages[ 1 ].first );
        ASSERT_FALSE( nullptr == senderReceiver1.sentMessages[ 1 ].second );
        ASSERT_EQ( 4, zmsg_size( senderReceiver1.sentMessages[ 1 ].second ) );
        ASSERT_EQ(
            0,
            zframe_size( zmsg_first( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            ClusterMessage::WorkerHeartbeat,
            frameHandler.get_frame_value< ClusterMessage >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            worker1->currentTime,
            frameHandler.get_frame_value< std::time_t >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
        ASSERT_EQ(
            worker1->selfHeartbeatDelay.count(),
            frameHandler.get_frame_value< Delay::rep >(
                zmsg_next( senderReceiver1.sentMessages[ 1 ].second ) ) );
    }

    // For all handle tests, test through handleBrokerMessage
    // want to ensure that ret code is always 0

    // All broker messages should be formatted as follows:
    //  1.  empty frame
    //  2.  ClusterMessage enum

    //Heartbeat format
    //  3.  std::time_t sentAt
    //  4.  int new brokerHeartbeatDelay
    //Can use the TestClientProxy function to create sample messages

    //New request format
    //  3.  one or more client envelopes
    //  4.  null frame
    //  5.  LHCVersionFlags
    //  5.  RequestType
    //  6.  RequestId
    //  8.  zero or more data framees

    void checkForwardedRequest(
        ZMQFrameHandler& frameHandler,
        ZMQMessage* forwardedMsg,
        const LHCVersionFlags& vfs,
        RequestType requestType,
        RequestId requestId,
        const vector< const char* >& dataFrames )
    {
        vector< const char* > empty;
        ZMQFrame* frame = nullptr;
        LHCVersionFlags msgVfs;
        RequestType msgRequestType;
        RequestId msgRequestId;

        frame = zmsg_first( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        msgVfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
        ASSERT_EQ( vfs, msgVfs );

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        msgRequestType = frameHandler.get_frame_value< RequestType >( frame );
        ASSERT_EQ( requestType, msgRequestType );


        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        msgRequestId = frameHandler.get_frame_value< RequestId >( frame );
        ASSERT_EQ( requestId, msgRequestId );

        for ( auto it = dataFrames.begin(); it != dataFrames.end(); ++it )
        {
            string data;
            frame = zmsg_next( forwardedMsg );
            ASSERT_TRUE( frame != nullptr );
            ASSERT_TRUE( zframe_size( frame ) > 0 );
            data = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), data ) << *it << " != " << data.c_str();
        }

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame == nullptr );
    }

    TEST_F( TestWorker, TestHandleBrokerHeartbeat )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 10000 } ) );
        vector< const char* > empty;
        std::time_t sentAt1 = 1;
        std::time_t sentAt2 = 20;
        std::time_t sentAt3 = 300;
        Delay newSendDelay1( 1000 );
        Delay newSendDelay2( 2000 );
        Delay newSendDelay3( 3000 );
        ClusterMessage brokerMessage = ClusterMessage::BrokerHeartbeat;
        uint32_t badBrokerMessage = 30000;
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
            empty,
            true,
            &brokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noDelim = createBrokerHeartbeat(
            frameHandler,
            empty,
            false,
            &brokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noMessageType = createBrokerHeartbeat(
            frameHandler,
            empty,
            true,
            nullptr,
            &sentAt1,
            &newSendDelay1 );

        badMessageType = createBrokerHeartbeat(
            frameHandler,
            empty,
            true,
            (ClusterMessage*)&badBrokerMessage,
            &sentAt1,
            &newSendDelay1 );

        noSentAt = createBrokerHeartbeat(
            frameHandler,
            empty,
            true,
            &brokerMessage,
            nullptr,
            &newSendDelay1 );

        noNewDelay = createBrokerHeartbeat(
            frameHandler,
            empty,
            true,
            &brokerMessage,
            &sentAt1,
            nullptr );

        goodHeartbeat2 = createBrokerHeartbeat(
            frameHandler,
            empty,
            true,
            &brokerMessage,
            &sentAt2,
            &newSendDelay2 );


        goodHeartbeat3 = createBrokerHeartbeat(
            frameHandler,
            empty,
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


        // null message, ClientProxyStat::FailedBrokerReceives
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::FailedBrokerReceives ] );

        // good heartbeat, WorkerStat::BrokerHeartbeatsReceived,
        // brokerLastActive == currentTime
        // new send delay
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        worker->brokerLastActive = 1;
        STATS_ASSERT_EQ(
            1,
            worker->stats[ WorkerStat::BrokerHeartbeatsReceived ] );
        ASSERT_EQ( newSendDelay1, worker->brokerHeartbeatDelay );

        // noDelim
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        STATS_ASSERT_EQ(
            1,
            worker->stats[ WorkerStat::MalformedBrokerMessages ] );

        // noMessageType
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        STATS_ASSERT_EQ(
            2,
            worker->stats[ WorkerStat::MalformedBrokerMessages ] );

        // badMessageType
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        STATS_ASSERT_EQ(
            3,
            worker->stats[ WorkerStat::MalformedBrokerMessages ] );

        // noSentAt
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        STATS_ASSERT_EQ(
            4,
            worker->stats[ WorkerStat::MalformedBrokerMessages ] );

        // noNewDelay
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        STATS_ASSERT_EQ(
            5,
            worker->stats[ WorkerStat::MalformedBrokerMessages ] );

        // goodHeartbeat2
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        worker->brokerLastActive = 1;
        ASSERT_EQ( newSendDelay2, worker->brokerHeartbeatDelay );
        STATS_ASSERT_EQ(
            4,
            worker->stats[ WorkerStat::BrokerHeartbeatsReceived ] );

        // goodHeartbeat3
        worker->currentTime = 0;
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_TRUE( worker->currentTime != 0 ) << "current time not updated";
        ASSERT_EQ( worker->brokerLastActive, worker->currentTime );
        ASSERT_EQ( newSendDelay3, worker->brokerHeartbeatDelay );
        STATS_ASSERT_EQ(
            5,
            worker->stats[ WorkerStat::BrokerHeartbeatsReceived ] );
    }


    // Handling a new request
    //   0.  init a new clientRquestContext
    //   1.  Parse the client address frames
    //       into the clientRequestContext
    //       AND a receipt message
    //       AND remove address + null delim from input message
    //   2.  Append Null, Broker::Receipt, Worker::Receipt, Null
    //       to receipt message
    //   3.  Parse RequestType and append to receipt and into context
    //   4.  Parse RequestId and append to receipt
    //       and into context
    // ! 5.  Send receipt - success => Inc(ReceiptsSent) else
    //       Inc(ReceiptsSentFailed), always continue
    //       Sent to messagingSocket
    //       The receipt is expected to be of form
    //       [Address]+ | NULL | BM:Receipt | WM:Receipt | RT | RID
    //   5.  Parse response type into context AND
    //       remove from message
    // ! 6.  Check redirect and if redirect update context AND RETURN
    //       and Inc(RedirectsReceived)
    // ! 7.  Send remnants of request message to requestProcessor
    //       via requestProcessor->getControllerSocket()
    //       should have form
    //       RT | RID | <data frames>
    //       ON FAIL => Inc(FailedProcessorSends) and buffer to be resent
    //   8.  Inc(RequestReceived)
    //   9   Sync =>  Inc(SyncRequestReceived) else Inc(AsyncResReceived)
    //   10. addNewInProgressRequest( RT/RID => context )

    // The function will only add to context on successful send
    // The function will only send receipt on successful send
    // The function will abort on any parse failure

    void checkWorkerInProgressRequests( const Worker& worker, RequestType rt, RequestId rid, uint32_t expectedNumInprogress )
    {
        auto it = worker.requestTypeContexts.find( rt );
        auto it2 = it->second.GetInProgressRequests().find( rid );
        ASSERT_TRUE(
            it !=
            worker.requestTypeContexts.end() );
        ASSERT_EQ( expectedNumInprogress, it->second.GetInProgressRequests().size() );
        ASSERT_TRUE( it2 != it->second.GetInProgressRequests().end() );
    }

    void checkWorkerInProgressRequests( const Worker& worker, RequestType rt, uint32_t expectedNumInprogress )
    {
        auto it = worker.requestTypeContexts.find( rt );
        ASSERT_TRUE(
            it !=
            worker.requestTypeContexts.end() );
        ASSERT_EQ( expectedNumInprogress, it->second.GetInProgressRequests().size() );
    }


    TEST_F( TestWorker, TestHandleBrokerNewRequest )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        //  CASES
        //  Parsing Errors
        //  1.  Missing client envelope
        //      -> Inc( MalformedRequest )
        ZMQMessage* noClientEnv( nullptr );

        //  2.  Missing NULL delimiter after evelope (i.e. <env> | rt | rid | rspt )
        //      -> Inc( MalformedRequest )
        ZMQMessage* noNullDelim( nullptr );

        //  3.  Missing RT (i.e. <env> | NULL | NULL )
        //      -> Inc( MalformedRequest )
        ZMQMessage* noRequestType( nullptr );

        //  4.  Missing RID (i.e. <env> | NULL | RT | NULL )
        //      -> Inc( MalformedRequest )
        ZMQMessage* noRequestID( nullptr );

        //  4.  Missing RSPT (i.e. <env> | NULL | RT | RID | NULL )
        //      -> Inc( MalformedRequest )
        ZMQMessage* noRespType( nullptr );

        //  5.  Bad Response Type
        //      -> ???
        ZMQMessage* badRespType( nullptr );

        ZMQMessage* unsupportedReqType( nullptr );

        //  Send Errors (0, -1)
        //  1.  Failed to Send Receipt e.e. normal
        //  ->  Inc( ReceiptSentFailed )
        //  ->  e.e. normal (check other stats, handler send, context)
        ZMQMessage* goodMsgForNoReceipt( nullptr );

        //  2.  Failed to Forward to Processor (1)
        //  ->  Inc( FailedProcessorSends )
        //  ->  Inc other stats
        //  ->  No receipt, no receipt stat
        //  ->  Not in context
        ZMQMessage* goodMsgForForwardFailed( nullptr );

        //  Normal Paths
        //  -> Validate context after each
        //  -> Validate Processor message after each
        //  1.  Multi Env - both parsed as expected into receipt, context
        ZMQMessage* goodMsgMultiEnv( nullptr );

        //  2.  Single Env - both parsed as expected into receipt, context
        ZMQMessage* goodMsgSingleEnv( nullptr );

        //  3.  Async
        //      -> Inc( AsyncRequestsReceived )
        ZMQMessage* goodMsgAsync( nullptr );

        //  4.  Sync
        //      -> Inc( SyncRequestsReceived )
        ZMQMessage* goodMsgSync( nullptr );

        //  7.  Multi Data
        ZMQMessage* goodMultiData( nullptr );

        //  8.  No Data
        ZMQMessage* goodMsgNoData( nullptr );

        //  9.  Request Redirected
        //      -> receipt sent
        //      -> nothing sent to handler
        //      -> request redirected successfully
        ZMQMessage* goodMsgForRedirect( nullptr );

        ClusterMessage brokerMessage = ClusterMessage::BrokerNewRequest;
        vector< const char* > emptyEnv;
        vector< const char* > singleEnv{ "clientA1" };
        vector< const char* > quadrupleEnv{
            "clientB1", "clientB2", "clientB3", "clientB4" };
        vector< const char* > toRedirectToEnv{
            "clientC1", "clientC2", "clientC3" };
        RequestType rt0( 0 );
        RequestType rt1( 1 );
        RequestType rt2( 22 );
        RequestType rt3( 333 );
        RequestType rtUnsupported( 999 );
        RequestId rid0( 0 );
        RequestId rid1( 100000009 );
        RequestId rid2( 203 );
        RequestId rid3( 30005 );
        LHCVersionFlags responseTypeSync( MessageFlag::ReceiptOnReceive );
        LHCVersionFlags responseTypeAsync(
            MessageFlag::Asynchronous | MessageFlag::ReceiptOnReceive );
        LHCVersionFlags responseTypeBad( MessageFlag::Asynchronous | 1111111 );
        vector< const char* > noData;
        vector< const char* > oneData{ "dataA1" };
        vector< const char* > someData{ "dataB1", "dataB2", "dataB3" };
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { rt0, rt1, rt2, rt3 } ) );

        noClientEnv = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            emptyEnv,
            true,
            &rt1,
            &rid1,
            &responseTypeAsync,
            oneData );
        noNullDelim = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            false,
            &rt1,
            &rid1,
            &responseTypeAsync,
            oneData );
        noRequestType = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            nullptr,
            &rid1,
            &responseTypeAsync,
            oneData );
        noRequestID = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt1,
            nullptr,
            &responseTypeAsync,
            oneData );
        noRespType = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt1,
            &rid1,
            nullptr,
            oneData );
        badRespType = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt0,
            &rid0,
            &responseTypeBad,
            oneData );
        unsupportedReqType = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rtUnsupported,
            &rid1,
            &responseTypeBad,
            oneData );
        goodMsgForNoReceipt = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt1,
            &rid1,
            &responseTypeAsync,
            oneData );
        goodMsgForForwardFailed = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt1,
            &rid2,
            &responseTypeAsync,
            oneData );
        goodMsgMultiEnv = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            quadrupleEnv,
            true,
            &rt1,
            &rid3,
            &responseTypeAsync,
            oneData );
        goodMsgSingleEnv = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid1,
            &responseTypeAsync,
            oneData );
        goodMsgAsync = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid2,
            &responseTypeAsync,
            oneData );
        goodMsgSync = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid3,
            &responseTypeSync,
            oneData );
        goodMultiData = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt3,
            &rid1,
            &responseTypeAsync,
            someData );
        goodMsgNoData = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt3,
            &rid2,
            &responseTypeAsync,
            noData );
        goodMsgForRedirect = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            toRedirectToEnv,
            true,
            &rt1,
            &rid1,
            &responseTypeSync,
            noData );

        senderReceiver.receiveReturnValues = {
            // parsing errors
            noClientEnv,
            noNullDelim,
            noRequestType,
            noRequestID,
            noRespType,
            badRespType,
            unsupportedReqType,
            // send errors
            goodMsgForNoReceipt,
            goodMsgForForwardFailed,
            // okay paths
            goodMsgMultiEnv,
            goodMsgSingleEnv,
            goodMsgAsync,
            goodMsgSync,
            goodMultiData,
            goodMsgNoData,
            goodMsgForRedirect
        };

        senderReceiver.sendReturnValues = {
            -1, // fail to send to handler
            0,  // succeeded sending to handler so that we can ..
            -1, // ... fail to send receipt to broker
            1,  // failed to send to handler
            0   // e.e. okay
        };


        // failed to parse due to no client env
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );    //parsing errors should always be ignored
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, 0 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, 0 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, 0 ) );

        // failed to parse due to unterminated env
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );    //parsing errors should always be ignored
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        // failed to parse due to no rt
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );    //parsing errors should always be ignored
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        // failed to parse due to no rid
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );    //parsing errors should always be ignored
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        // failed to parse due to no resp type
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );    //parsing errors should always be ignored
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        // bad resp type is interpreted as an asynca, send fail results in an exit
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( WorkerExitState::ProcessorInaccessable, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 6, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::FailedProcessorSends ] );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        worker->setExitState( WorkerExitState::None );  // reset exit state

        // unsupported request type should result in an exit
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( WorkerExitState::ReceivedUnsupportedRequest, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 7, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::FailedProcessorSends ] );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 0, worker->requestContexts.size() );

        worker->setExitState( WorkerExitState::None );  // reset exit state

        // only receipt should fail
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 8, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 1, worker->requestContexts.size() );
        ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 1 ) );
        ASSERT_EQ( (TestZMQSocket*)worker->requestProcessorMessagingSocket,
            senderReceiver.sentMessages[ 1 ].first );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 1 ].second,
                responseTypeAsync, rt1, rid1, oneData ) );

        /*
        goodMsgForForwardFailed = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt1,
            &rid2,
            &responseTypeAsync,
            oneData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( WorkerExitState::ProcessorInaccessable, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 9, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::FailedProcessorSends ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 4, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 1, worker->requestContexts.size() );

        worker->setExitState( WorkerExitState::None );  // reset exit state

        /*
        goodMsgMultiEnv = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            quadrupleEnv,
            true,
            &rt1,
            &rid3,
            &responseTypeAsync,
            oneData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 10, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 2, worker->requestContexts.size() );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 4 ].second,
                responseTypeAsync, rt1, rid3, oneData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 5 ].second, rt1, rid3, quadrupleEnv ) );


        /*
        goodMsgSingleEnv = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid1,
            &responseTypeAsync,
            oneData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 11, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 3, worker->requestContexts.size() );
        ASSERT_EQ( 8, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 1 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 6 ].second,
                responseTypeAsync, rt2, rid1, oneData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 7 ].second, rt2, rid1, singleEnv ) );

        /*
        goodMsgAsync = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid2,
            &responseTypeAsync,
            oneData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 12, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 6, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 4, worker->requestContexts.size() );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid2, 2 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 8 ].second,
                responseTypeAsync, rt2, rid2, oneData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 9 ].second, rt2, rid2, singleEnv ) );

        /*
        goodMsgSync = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt2,
            &rid3,
            &responseTypeSync,
            oneData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 13, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 6, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 5, worker->requestContexts.size() );
        ASSERT_EQ( 12, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid2, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid3, 3 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 10 ].second,
                responseTypeSync, rt2, rid3, oneData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 11 ].second, rt2, rid3, singleEnv ) );

        /*
        goodMultiData = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt3,
            &rid1,
            &responseTypeAsync,
            someData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 14, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 7, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 6, worker->requestContexts.size() );
        ASSERT_EQ( 14, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid2, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid3, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, rid1, 1 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 12 ].second,
                responseTypeAsync, rt3, rid1, someData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 13 ].second, rt3, rid1, singleEnv ) );

        /*
        goodMsgNoData = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            singleEnv,
            true,
            &rt3,
            &rid2,
            &responseTypeAsync,
            noData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 15, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 6, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 8, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        ASSERT_EQ( 7, worker->requestContexts.size() );
        ASSERT_EQ( 16, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid2, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid3, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, rid2, 2 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedRequest(
                frameHandler, senderReceiver.sentMessages[ 14 ].second,
                responseTypeAsync, rt3, rid2, noData ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 15 ].second, rt3, rid2, singleEnv ) );

        /*
        goodMsgForRedirect = createBrokerNewRequest(
            frameHandler,
            true,
            &brokerMessage,
            toRedirectToEnv,
            true,
            &rt1,
            &rid1,
            &responseTypeSync,
            noData );
        */
        ret = worker->handleBrokerMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 5, worker->stats[ WorkerStat::MalformedBrokerMessages ] );
        STATS_ASSERT_EQ( 16, worker->stats[ WorkerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 7, worker->stats[ WorkerStat::ReceiptsSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::ReceiptsSentFailed ] );
        STATS_ASSERT_EQ( 8, worker->stats[ WorkerStat::AsyncRequestsReceived ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::SyncRequestsReceived ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RedirectsReceived ] );
        ASSERT_EQ( 7, worker->requestContexts.size() );
        ASSERT_EQ( 17, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt1, rid3, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid1, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid2, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt2, rid3, 3 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, rid1, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkWorkerInProgressRequests( *worker, rt3, rid2, 2 ) );
        ASSERT_NO_FATAL_FAILURE( checkReceipt( frameHandler, senderReceiver.sentMessages[ 16 ].second, rt1, rid1, toRedirectToEnv ) );
    }

    TEST_F( TestWorker, TestCheckBrokerPulse )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver, { 3 } ) );
        int ret = 0;

        worker->brokerLastActive = 10;
        worker->brokerDeathDelay = Delay( 26 );

        // If broker is not alive as of the current time then expect
        //  1) return <0, 2) exitState == BrokerDied, 3) IncStat( BrokerDeaths )
        ASSERT_TRUE( worker->isBrokerAliveAsOf( 20 ) );
        worker->currentTime = 20;
        ret = worker->checkBrokerPulse();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( WorkerExitState::None, worker->exitState );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::BrokerDeaths ] );

        ASSERT_FALSE( worker->isBrokerAliveAsOf( 37 ) );
        worker->currentTime = 37;
        ret = worker->checkBrokerPulse();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( WorkerExitState::BrokerDied, worker->exitState );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::BrokerDeaths ] );
    }

    void setupRequestContexts(
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        const vector< const char* > env,
        bool inProgress, bool rContext )
    {
        ZMQFrameHandler frameHandler;
        if ( inProgress )
        {
            auto it = worker.requestTypeContexts.find( requestType );
            if ( it != worker.requestTypeContexts.end() )
                it->second.AddInProgress( requestId );
        }

        if ( rContext )
        {
            ClientEnvelope cenv;

            ZMQMessage* msg = zmsg_new();
            for ( auto it = env.rbegin(); it != env.rend(); ++it )
                frameHandler.prepend_message_value< string >( msg, string( *it ) );

            for ( auto it = env.begin(); it != env.end(); ++it )
            {
                ZMQFrame* frame = zmsg_pop( msg );
                cenv.push_back( frame );
            }

            zmsg_destroy( &msg );

            worker.requestContexts.insert(
                std::pair<
                std::pair< RequestType, RequestId >,
                ClientRequestContext&& >(
                    std::pair< RequestType, RequestId>(
                        requestType, requestId ),
                    ClientRequestContext(
                        requestType,
                        requestId,
                        vfs,
                        move( cenv ) ) ) );
        }
    }

    void setupRequestContexts(
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        bool inProgress, bool rContext )
    {
        vector< const char* > env;
        setupRequestContexts(
            worker,
            requestType,
            requestId,
            vfs,
            env,
            inProgress,
            rContext );
    }

    void checkResponseMessage(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* zmqMessage,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        RequestStatus requestStatus,
        const vector< const char* >& env,
        const vector< const char* >& data )
    {
        ZMQFrame* frame( nullptr );
        ClusterMessage expectedWorkerMessage = ClusterMessage::WorkerFinished;
        unsigned int expectedSize = 7 + env.size() + data.size();

        // if( ! vfs.hasAny( MessageFlag::Asynchronous ) )
        ++expectedSize; // for broker message

        ASSERT_TRUE( nullptr != zmqMessage )
            << "failed at line " << line;
        ASSERT_EQ( expectedSize, zmsg_size( zmqMessage ) )
            << "failed at line " << line;
        // null | wm::finished[a]sync | [env] | null | bm::resposne | vfs | type | id | response [ | data ] *

        // null frame
        frame = zmsg_first( zmqMessage );
        ASSERT_EQ( 0, zframe_size( frame ) )
            << "failed at line " << line;

        // ClusterMessage::WorkerFinished
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( expectedWorkerMessage, frameHandler.get_frame_value< ClusterMessage >( frame ) )
            << "failed at line " << line;

        // client env
        for ( auto it = env.begin(); it != env.end(); ++it )
        {
            frame = zmsg_next( zmqMessage );
            ASSERT_EQ( string( *it ), frameHandler.get_frame_value< string >( frame ) )
                << "failed at line " << line;
        }

        // null delim of client env
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( 0, zframe_size( frame ) )
            << "failed at line " << line;

        // if( ! vfs.hasAny( MessageFlag::Asynchronous ) )
        // {
            // broker message
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( ClusterMessage::BrokerResponse,
            frameHandler.get_frame_value< ClusterMessage >( frame ) )
            << "failed at line " << line;
        // }

        // vfs
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( vfs, frameHandler.get_frame_value< LHCVersionFlags >( frame ) )
            << "failed at line " << line;

        // type
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( requestType, frameHandler.get_frame_value< RequestType >( frame ) )
            << "failed at line " << line;

        // id
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( requestId, frameHandler.get_frame_value< RequestId >( frame ) )
            << "failed at line " << line;

        // status
        frame = zmsg_next( zmqMessage );
        ASSERT_EQ( requestStatus, frameHandler.get_frame_value< RequestStatus >( frame ) )
            << "failed at line " << line;

        // data
        for ( auto it = data.begin(); it != data.end(); ++it )
        {
            frame = zmsg_next( zmqMessage );
            ASSERT_EQ( string( *it ), frameHandler.get_frame_value< string >( frame ) )
                << "failed at line " << line;
        }
    }


    TEST_F( TestWorker, TestHandleProcessorMessage )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        // The worker will do the following:
        //  1) Parse the request type
        //  2) Parse the request id
        //  TODO - check for a request status
        //  3) Lookup the request context and request type context
        //  4) Append a null delimiter
        //  4) If Sync, prepend the broker message
        //  5) Prepend each frame in the requestContext.envelope
        //  6) Prepend FinishedSync or FinishedAsync AND IncStat()
        //  7) Prepend a null delimiter to mark the end of envelope
        //  8) Send, inc stat correspondingly on success/fail
        //  9) Erase the inprogress request and request context
        //     if available
        //  TODO - ensure that if one is not there then the other
        //         is still deleted
        // 10) Override positive return values to zero, pass negative
        //     Values through

        ZMQMessage* noRequestType( nullptr );
        ZMQMessage* noRequestId( nullptr );
        ZMQMessage* noStatus( nullptr );
        ZMQMessage* noRTCAndNoRC( nullptr );
        ZMQMessage* noRTCAndHasRC( nullptr );
        ZMQMessage* hasRTCAndNoIPAndNoRC( nullptr );
        ZMQMessage* hasRTCAndIPAndNoRC( nullptr );
        ZMQMessage* hasRTCAndNoIPAndHasRC( nullptr );
        ZMQMessage* goodForSendFail( nullptr );
        ZMQMessage* goodSyncNoData( nullptr );
        ZMQMessage* goodSyncSomeData( nullptr );
        ZMQMessage* goodAsyncNoData( nullptr );
        ZMQMessage* goodAsyncSomeData( nullptr );
        ZMQMessage* noVFS( nullptr ); // TODO - vfs

        RequestType missingRT1( 100 );
        RequestId missingRID1( 1000 );

        RequestType missingRT2( 101 );
        RequestId hasRID1( 1001 );

        RequestType hasRT1And2And3( 102 );
        RequestId missingRID2( 1002 );
        RequestId missingRID3( 1003 );
        RequestId hasRID2( 1004 );

        LHCVersionFlags responseTypeAsync( MessageFlag::Asynchronous );
        LHCVersionFlags responseTypeSync;
        RequestType requestType1( 1 );
        RequestType requestType4( 40 );
        RequestType requestType5( 5 );
        RequestId requestID1( 300000 );
        RequestId requestID2( 300000 );
        RequestId requestID3( 300001 );
        RequestId requestID4( 300001 );
        RequestStatus statusSucceeded( RequestStatus::Succeeded );
        RequestStatus statusFailed( RequestStatus::Failed );

        vector< const char* > noData;
        vector< const char* > someData1{
            "data1", "data2", "data3"
        };
        vector< const char* > someData2{
            "info1", "info2", "info3"
        };

        vector< const char* > env1{
            "env1a", "env1b"
        };

        vector< const char* > env2{
            "env2a"
        };

        vector< const char* > env3{
            "env3a", "env3b", "env3c"
        };

        vector< const char* > env4{
            "env4a", "env4b", "env4c", "env4d"
        };

        vector< const char* > env5{
            "env5a"
        };


        senderReceiver.sendReturnValues = { 1, 0 };

        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver,
                { requestType1, hasRT1And2And3, requestType4, requestType5 } ) );


        noRequestType = createRequestHandlerResponse(
            frameHandler, nullptr, &requestID1, &statusSucceeded, noData,
            &responseTypeAsync
        );

        noRequestId = createRequestHandlerResponse(
            frameHandler, &requestType1, nullptr, &statusSucceeded, noData,
            &responseTypeAsync
        );

        noStatus = createRequestHandlerResponse(
            frameHandler, &requestType1, &requestID1, nullptr, noData,
            &responseTypeAsync
        );

        noVFS = createRequestHandlerResponse(
            frameHandler, &requestType1, &requestID1, &statusSucceeded, noData,
            nullptr
        );

        noRTCAndNoRC = createRequestHandlerResponse(
            frameHandler, &missingRT1, &missingRID1, &statusSucceeded, noData,
            &responseTypeAsync
        );

        // missingRT2 does not have a request type context,
        // but hasRID1,missingRT2 has a request context
        noRTCAndHasRC = createRequestHandlerResponse(
            frameHandler, &missingRT2, &hasRID1, &statusSucceeded, noData,
            &responseTypeAsync
        );


        hasRTCAndNoIPAndNoRC = createRequestHandlerResponse(
            frameHandler, &hasRT1And2And3, &missingRID1, &statusSucceeded, noData,
            &responseTypeAsync
        );

        // hasRT1And2And3 has a request type context and missingRID2 is in progress
        // but hasRT1And2And3, missingRID2 does not have a request context
        hasRTCAndIPAndNoRC = createRequestHandlerResponse(
            frameHandler, &hasRT1And2And3, &missingRID2, &statusSucceeded, noData,
            &responseTypeAsync
        );

        // hasRT1And2And3 has a request type context and missingRID3 is not in progress
        // but hasRT1And2And3, missingRID3 has a request context
        hasRTCAndNoIPAndHasRC = createRequestHandlerResponse(
            frameHandler, &hasRT1And2And3, &missingRID3, &statusSucceeded, noData,
            &responseTypeAsync
        );

        goodForSendFail = createRequestHandlerResponse(
            frameHandler, &requestType1, &hasRID2, &statusSucceeded, noData,
            &responseTypeAsync
        );

        goodSyncNoData = createRequestHandlerResponse(
            frameHandler, &requestType5, &requestID1, &statusSucceeded, noData,
            &responseTypeSync
        );

        goodSyncSomeData = createRequestHandlerResponse(
            frameHandler, &requestType4, &requestID2, &statusFailed, someData1,
            &responseTypeSync
        );

        goodAsyncNoData = createRequestHandlerResponse(
            frameHandler, &requestType4, &requestID3, &statusFailed, noData,
            &responseTypeAsync
        );

        goodAsyncSomeData = createRequestHandlerResponse(
            frameHandler, &requestType5, &requestID4, &statusSucceeded, someData2,
            &responseTypeAsync
        );

        senderReceiver.receiveReturnValues = {
            noRequestType,
            noRequestId,
            noStatus,
            noRTCAndNoRC,
            noRTCAndHasRC,
            hasRTCAndNoIPAndNoRC,
            hasRTCAndIPAndNoRC,
            hasRTCAndNoIPAndHasRC,
            goodForSendFail,
            goodSyncNoData,
            goodSyncSomeData,
            goodAsyncNoData,
            goodAsyncSomeData,
            noVFS
        };

        setupRequestContexts(
            *worker, missingRT2, hasRID1, responseTypeAsync, false, true );
        setupRequestContexts(
            *worker, hasRT1And2And3, missingRID2, responseTypeAsync, true, false );
        setupRequestContexts(
            *worker, hasRT1And2And3, missingRID3, responseTypeAsync, false, true );
        setupRequestContexts(
            *worker, hasRT1And2And3, hasRID2, responseTypeAsync, false, true );
        setupRequestContexts(
            *worker, requestType1, hasRID2, responseTypeAsync, env1, true, true );
        setupRequestContexts(
            *worker, requestType5, requestID1, responseTypeSync, env2, true, true );
        setupRequestContexts(
            *worker, requestType4, requestID2, responseTypeSync, env3, true, true );
        setupRequestContexts(
            *worker, requestType4, requestID3, responseTypeAsync, env4, true, true );
        setupRequestContexts(
            *worker, requestType5, requestID4, responseTypeAsync, env5, true, true );

        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, missingRT2, hasRID1, false, false, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, hasRT1And2And3, missingRID2, true, true, false ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, hasRT1And2And3, missingRID3, true, false, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, hasRT1And2And3, hasRID2, true, false, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType1, hasRID2, true, true, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType5, requestID1, true, true, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType4, requestID2, true, true, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType4, requestID3, true, true, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType5, requestID4, true, true, true ) );

#define badContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, missingRT2, hasRID1, false, false, true ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID2, true, true, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID3, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, hasRID2, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType1, hasRID2, true, true, true )

#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID1, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID4, true, true, true )

        // Cases
        //  1. No RequestType
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::MalformedProcessorResponse ] );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  2. No RequestId
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MalformedProcessorResponse ] );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  3. No ResponseType
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MalformedProcessorResponse ] );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::SyncRequestsCompleted ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::RequestsCompletedSentFailed ] );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::RequestsCompletedSent ] );



        //  4. No rt context and no request context
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::MissingRequestContext ] );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  5. No rt context but has request context
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::MissingRequestContext ] );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, missingRT2, hasRID1, false, false, false ) );
#undef badContextsUnchanged
#define badContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, missingRT2, hasRID1, false, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID2, true, true, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID3, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, hasRID2, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType1, hasRID2, true, true, true );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );


        //  6. Has rt context but not in progress and no request context
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestContext ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::CompletedRequestsNotInProgress ] );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  7. Has rt context and in progress and no request context
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MissingRequestContext ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::CompletedRequestsNotInProgress ] );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, hasRT1And2And3, missingRID2, true, false, false ) );
#undef badContextsUnchanged
#define badContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, missingRT2, hasRID1, false, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID2, true, false, false );  \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID3, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, hasRID2, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType1, hasRID2, true, true, true );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  8. Has rt context but not in progress and has request context
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MissingRequestContext ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::CompletedRequestsNotInProgress ] );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, hasRT1And2And3, missingRID3, true, false, false ) );
#undef badContextsUnchanged
#define badContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, missingRT2, hasRID1, false, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID3, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, hasRID2, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType1, hasRID2, true, true, true );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        //  9. Send Fails - e.e. still cleaned up
        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 0, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MissingRequestContext ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::CompletedRequestsNotInProgress ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RequestsCompletedSentFailed ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType1, hasRID2, true, false, false ) );
#undef badContextsUnchanged
#define badContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, missingRT2, hasRID1, false, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, missingRID3, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, hasRT1And2And3, hasRID2, true, false, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType1, hasRID2, true, false, false );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        /*
                env2
                goodSyncNoData = createRequestHandlerResponse(
                    frameHandler, &requestType5, &requestID1, &statusSucceeded, noData
                );

                env3
                goodSyncSomeData = createRequestHandlerResponse(
                    frameHandler, &requestType4, &requestID2, &statusFailed, someData1
                );

                env4
                goodAsyncNoData = createRequestHandlerResponse(
                    frameHandler, &requestType4, &requestID3, &statusFailed, noData
                );

                env5
                goodAsyncSomeData = createRequestHandlerResponse(
                    frameHandler, &requestType5, &requestID4, &statusSucceeded, someData2
                );
        */

        // 10. Good, Sync no data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::SyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 1 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 1 ].second,
            requestType5,
            requestID1,
            responseTypeSync,
            statusSucceeded,
            env2,
            noData ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType5, requestID1, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID1, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID4, true, true, true )
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        // 11. Good, Sync with data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::SyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 2 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 2 ].second,
            requestType4,
            requestID2,
            responseTypeSync,
            statusFailed,
            env3,
            someData1 ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType4, requestID2, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID1, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID4, true, true, true )
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );


        // 12. Good, Async no data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 4, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 3 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 3 ].second,
            requestType4,
            requestID3,
            responseTypeAsync,
            statusFailed,
            env4,
            noData ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType4, requestID3, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID1, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID4, true, true, true )
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        // 13. Good, Async with data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 5, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 4 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 4 ].second,
            requestType5,
            requestID4,
            responseTypeAsync,
            statusSucceeded,
            env5,
            someData2 ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType5, requestID4, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID1, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType5, requestID4, true, false, false )
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        // 13. Missing vfs
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 5, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType5, requestID4, true, false, false ) );
        ASSERT_NO_FATAL_FAILURE( badContextsUnchanged( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );
        STATS_ASSERT_EQ( 4, worker->stats[ WorkerStat::MalformedProcessorResponse ] );


        // TODO - fatal error returns -1


        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::MissingRequestTypeContext ] );
        STATS_ASSERT_EQ( 3, worker->stats[ WorkerStat::MissingRequestContext ] );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::CompletedRequestsNotInProgress ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RequestsCompletedSentFailed ] );
    }

    TEST_F( TestWorker, TestExitOnFatalError )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        ZMQMessage* goodSyncSomeData( nullptr );
        ZMQMessage* goodAsyncNoData( nullptr );

        LHCVersionFlags responseTypeAsync( MessageFlag::Asynchronous );
        LHCVersionFlags responseTypeSync;
        RequestType requestType4( 40 );
        RequestId requestID2( 300000 );
        RequestId requestID3( 300001 );
        RequestStatus statusFailed( RequestStatus::FatalError );

        vector< const char* > noData;
        vector< const char* > someData1{
            "data1", "data2", "data3"
        };

        vector< const char* > env3{
            "env3a", "env3b", "env3c"
        };

        vector< const char* > env4{
            "env4a", "env4b", "env4c", "env4d"
        };

        senderReceiver.sendReturnValues = { 0, 0 };

        unique_ptr< Worker > worker(
            createWorker( socketFactory, senderReceiver,
                { requestType4 } ) );

        goodSyncSomeData = createRequestHandlerResponse(
            frameHandler, &requestType4, &requestID2, &statusFailed, someData1,
            &responseTypeSync
        );

        goodAsyncNoData = createRequestHandlerResponse(
            frameHandler, &requestType4, &requestID3, &statusFailed, noData,
            &responseTypeAsync
        );

        senderReceiver.receiveReturnValues = {
            goodSyncSomeData,
            goodAsyncNoData
        };

        setupRequestContexts(
            *worker, requestType4, requestID2, responseTypeSync, env3, true, true );
        setupRequestContexts(
            *worker, requestType4, requestID3, responseTypeAsync, env4, true, true );

        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType4, requestID2, true, true, true ) );
        ASSERT_NO_FATAL_FAILURE(
            checkRTCAndIPAndRC(
                __LINE__, *worker, requestType4, requestID3, true, true, true ) );

#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, true, true ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, true, true );

        // Cases
        // 1. Good, Sync with data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::SyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 0 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 0 ].second,
            requestType4,
            requestID2,
            responseTypeSync,
            statusFailed,
            env3,
            someData1 ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType4, requestID2, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, true, true );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );

        // 11. Good, Async no data
        ret = worker->handleProcessorMessage();
        ASSERT_EQ( -1, ret );
        ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 2, worker->stats[ WorkerStat::RequestsCompletedSent ] );
        STATS_ASSERT_EQ( 1, worker->stats[ WorkerStat::AsyncRequestsCompleted ] );
        ASSERT_EQ( (TestZMQSocket*)worker->messagingSocket,
            senderReceiver.sentMessages[ 1 ].first );
        ASSERT_NO_FATAL_FAILURE( checkResponseMessage(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 1 ].second,
            requestType4,
            requestID3,
            responseTypeAsync,
            statusFailed,
            env4,
            noData ) );
        ASSERT_NO_FATAL_FAILURE( checkRTCAndIPAndRC( __LINE__, *worker, requestType4, requestID3, true, false, false ) );
#undef goodContextsUnchanged
#define goodContextsUnchanged( line ) \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID2, true, false, false ); \
        checkRTCAndIPAndRC( line, *worker, requestType4, requestID3, true, false, false );
        ASSERT_NO_FATAL_FAILURE( goodContextsUnchanged( __LINE__ ) );
    }
}
