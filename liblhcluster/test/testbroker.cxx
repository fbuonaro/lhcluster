#include <lhcluster_impl/broker.h>

#include "testzmqsocketutils.h"
#include "test.h"

#include <gtest/gtest.h>
#include <memory>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    class TestBroker : public ::testing::Test
    {
        protected:

            void SetUp()
            {

            }

            void Tear()
            {
            }
    };

                                
    unique_ptr< WorkerInfo > createWorkerInfo(
        ZMQFrameHandler& frameHandler,
        const char* identity,
        unsigned int freeCapacity,
        const vector< RequestType >& supportedRequestTypes,
        Delay heartbeatSendDelay,
        time_t brokerLastActive,
        time_t workerLastActive )
    {
        unique_ptr< WorkerInfo > workerInfo( nullptr );
        ZMQMessage* tmpMsg = zmsg_new();
        ZMQFrame* identityFrame = nullptr;
        LHCVersionFlags vfs;

        frameHandler.append_message_value( tmpMsg, string( identity ) );
        identityFrame = zmsg_pop( tmpMsg );
        zmsg_destroy( &tmpMsg );

        workerInfo = unique_ptr< WorkerInfo >( new WorkerInfo(
            &identityFrame,
            std::time( NULL ),
            freeCapacity,
            supportedRequestTypes,
            heartbeatSendDelay,
            vfs ) );
        workerInfo->UpdateBrokerLastActive( brokerLastActive );
        workerInfo->UpdateWorkerLastActive( workerLastActive );

        return move( workerInfo );
    }

    TEST_F( TestBroker, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfFrontendMessagingEndpoint( EndpointType::TCP, "selfFrontendMessagingEndpoint" );
        Endpoint selfBackendMessagingEndpoint( EndpointType::TCP, "selfBackendMessagingEndpoint" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfControllerEndpoint" );
        Delay selfHeartbeatDelay( 8 );
        Delay minimumWorkerHeartbeatDelay( 16 );
        Delay workerPulseDelay( 32 );
        TestZMQSocket* backendMessagingSocket( nullptr );
        TestZMQSocket* frontendMessagingSocket( nullptr );
        TestZMQSocket* listenerSocket( nullptr );
        TestZMQSocket* signalerSocket( nullptr );

        try
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        Broker broker( selfFrontendMessagingEndpoint,
                       selfBackendMessagingEndpoint,
                       selfControllerEndpoint,
                       selfHeartbeatDelay,
                       minimumWorkerHeartbeatDelay,
                       workerPulseDelay,
                       move( lpSocketFactory ),
                       move( lpSenderReceiver ) );

        ASSERT_EQ( 0, broker.workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ( 0, broker.requestTypeContextLookup.size() );

        backendMessagingSocket = ( TestZMQSocket* ) broker.backendMessagingSocket;
        frontendMessagingSocket = ( TestZMQSocket* ) broker.frontendMessagingSocket;
        listenerSocket = ( TestZMQSocket* ) broker.listenerSocket;
        signalerSocket = ( TestZMQSocket* ) broker.signalerSocket;

        ASSERT_EQ( 0, backendMessagingSocket->binded.size() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );
        ASSERT_EQ( 0, frontendMessagingSocket->binded.size() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );
        ASSERT_EQ( 0, listenerSocket->binded.size() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        ASSERT_EQ( 0, signalerSocket->connected.size() );

        // activateControllerSockets only binds/connects listener/signaler
        broker.activateControllerSockets();
        
        ASSERT_EQ( 0, backendMessagingSocket->binded.size() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );

        ASSERT_EQ( 0, frontendMessagingSocket->binded.size() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );

        ASSERT_TRUE( listenerSocket->binded.find( selfControllerEndpoint.path() ) !=
                     listenerSocket->binded.end() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );

        ASSERT_TRUE( signalerSocket->connected.find( selfControllerEndpoint.path() ) !=
                     signalerSocket->connected.end() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        

        // activateMessagingSockets only binds messaging sockets
        broker.activateMessagingSockets();

        ASSERT_TRUE( backendMessagingSocket->binded.find( selfBackendMessagingEndpoint.path() ) !=
                      backendMessagingSocket->binded.end() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );

        ASSERT_TRUE( frontendMessagingSocket->binded.find( selfFrontendMessagingEndpoint.path() ) !=
                      frontendMessagingSocket->binded.end() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );
        
        // deactivateControllerSockets only disconnects/unbinds signaler/listener
        broker.deactivateControllerSockets();
        
        ASSERT_TRUE( backendMessagingSocket->binded.find( selfBackendMessagingEndpoint.path() ) !=
                      backendMessagingSocket->binded.end() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );

        ASSERT_TRUE( frontendMessagingSocket->binded.find( selfFrontendMessagingEndpoint.path() ) !=
                      frontendMessagingSocket->binded.end() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );

        ASSERT_TRUE( listenerSocket->unBinded.find( selfControllerEndpoint.path() ) !=
                     listenerSocket->unBinded.end() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );
        ASSERT_EQ( 1, listenerSocket->binded.size() );

        ASSERT_TRUE( signalerSocket->disConnected.find( selfControllerEndpoint.path() ) !=
                     signalerSocket->disConnected.end() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        ASSERT_EQ( 1, signalerSocket->connected.size() );

        // deactivateMessagingSockets only unbinds messaging sockets

        broker.deactivateMessagingSockets();
        ASSERT_TRUE( listenerSocket->unBinded.find( selfControllerEndpoint.path() ) !=
                     listenerSocket->unBinded.end() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );
        ASSERT_EQ( 1, listenerSocket->binded.size() );

        ASSERT_TRUE( signalerSocket->disConnected.find( selfControllerEndpoint.path() ) !=
                     signalerSocket->disConnected.end() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        ASSERT_EQ( 1, signalerSocket->connected.size() );


        ASSERT_TRUE( backendMessagingSocket->unBinded.find( selfBackendMessagingEndpoint.path() ) !=
                      backendMessagingSocket->unBinded.end() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );
        ASSERT_EQ( 1, backendMessagingSocket->binded.size() );

        ASSERT_TRUE( frontendMessagingSocket->unBinded.find( selfFrontendMessagingEndpoint.path() ) !=
                      frontendMessagingSocket->unBinded.end() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );
        ASSERT_EQ( 1, frontendMessagingSocket->binded.size() );

        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception in Broker constructor/activation";
        }


        ASSERT_TRUE( listenerSocket && listenerSocket->destroyed );
        ASSERT_TRUE( listenerSocket->unBinded.find( selfControllerEndpoint.path() ) !=
                     listenerSocket->unBinded.end() );
        ASSERT_EQ( 0, listenerSocket->connected.size() );
        ASSERT_EQ( 1, listenerSocket->binded.size() );

        ASSERT_TRUE( signalerSocket && signalerSocket->destroyed );
        ASSERT_TRUE( signalerSocket->disConnected.find( selfControllerEndpoint.path() ) !=
                     signalerSocket->disConnected.end() );
        ASSERT_EQ( 0, signalerSocket->binded.size() );
        ASSERT_EQ( 1, signalerSocket->connected.size() );


        ASSERT_TRUE( backendMessagingSocket && backendMessagingSocket->destroyed );
        ASSERT_TRUE( backendMessagingSocket->unBinded.find( selfBackendMessagingEndpoint.path() ) !=
                      backendMessagingSocket->unBinded.end() );
        ASSERT_EQ( 0, backendMessagingSocket->connected.size() );
        ASSERT_EQ( 1, backendMessagingSocket->binded.size() );

        ASSERT_TRUE( frontendMessagingSocket && frontendMessagingSocket->destroyed );
        ASSERT_TRUE( frontendMessagingSocket->unBinded.find( selfFrontendMessagingEndpoint.path() ) !=
                      frontendMessagingSocket->unBinded.end() );
        ASSERT_EQ( 0, frontendMessagingSocket->connected.size() );
        ASSERT_EQ( 1, frontendMessagingSocket->binded.size() );
    }

    unique_ptr< Broker > createBroker(
        TestZMQSocketFactory& socketFactory,
        TestZMQSocketSenderReceiver& senderReceiver )
    {
        Endpoint selfFrontendMessagingEndpoint( EndpointType::TCP, "selfFrontendMessagingEndpoint" );
        Endpoint selfBackendMessagingEndpoint( EndpointType::TCP, "selfBackendMessagingEndpoint" );
        Endpoint selfControllerEndpoint( EndpointType::TCP, "selfControllerEndpoint" );
        Delay selfHeartbeatDelay( 8 );
        Delay minimumWorkerHeartbeatDelay( 4 );
        Delay workerPulseDelay( 32 );
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        return unique_ptr< Broker >( new Broker(
            selfFrontendMessagingEndpoint,
            selfBackendMessagingEndpoint,
            selfControllerEndpoint,
            selfHeartbeatDelay,
            minimumWorkerHeartbeatDelay,
            workerPulseDelay,
            move( lpSocketFactory ),
            move( lpSenderReceiver ) ) );
    }

    unique_ptr< WorkerInfo > createWorkerInfo(
        ZMQFrameHandler& zmqFrameHandler,
        const char* identity,
        unsigned int freeCapacity,
        const vector< RequestType >& supportedRequestTypes )
    {
        Delay heartbeatSendDelay( 10 );
        time_t brokerLastActive( std::time( NULL ) );
        time_t workerLastActive( brokerLastActive );

        return createWorkerInfo(
            zmqFrameHandler, identity, freeCapacity, supportedRequestTypes,
            heartbeatSendDelay, brokerLastActive, workerLastActive );
    }
 
    unique_ptr< WorkerInfo > createWorkerInfo(
        ZMQFrameHandler& zmqFrameHandler,
        const char* identity,
        unsigned int freeCapacity,
        const vector< RequestType >& supportedRequestTypes,
        Delay heartbeatSendDelay,
        time_t brokerLastActive )
    {
        time_t workerLastActive( brokerLastActive );

        return createWorkerInfo(
            zmqFrameHandler, identity, freeCapacity, supportedRequestTypes,
            heartbeatSendDelay, brokerLastActive, workerLastActive );
    }
 
    unique_ptr< WorkerInfo > createWorkerInfo(
        ZMQFrameHandler& zmqFrameHandler,
        const char* identity,
        unsigned int freeCapacity,
        const vector< RequestType >& supportedRequestTypes,
        time_t workerLastActive )
    {
        Delay heartbeatSendDelay( 1 );
        time_t brokerLastActive( workerLastActive );

        return createWorkerInfo(
            zmqFrameHandler, identity, freeCapacity, supportedRequestTypes,
            heartbeatSendDelay, brokerLastActive, workerLastActive );
    }
 
    void checkBrokerRequestTypeContextDeaths(
        unsigned int line,
        const Broker& broker,
        RequestType requestType,
        unsigned int expectedNumWorkerDeaths )
    {
        auto it = broker.requestTypeContextLookup.find( requestType );
        ASSERT_TRUE( it != broker.requestTypeContextLookup.end() ) << "failure at line " << line;
        STATS_ASSERT_EQ2(
            expectedNumWorkerDeaths,
            it->second.stats[ RequestTypeStat::WorkersDied ],
            "failure at line " << line );
    }

    void checkBrokerRequestTypeContext(
        unsigned int line,
        const Broker& broker,
        RequestType requestType,
        unsigned int expectedNumWorkersTotal,
        unsigned int expectedNumWorkersRegistered,
        unsigned int expectedNumWorkersAvail,
        unsigned int expectedNumRequests,
        WorkerInfo* expectedNextAvailWorker )
    {
        auto it = broker.requestTypeContextLookup.find( requestType );
        ASSERT_TRUE( it != broker.requestTypeContextLookup.end() ) << "failure at line " << line;

        ASSERT_EQ( expectedNumWorkersTotal,
                   it->second.NumWorkersTotal() ) << "failure at line " << line;
        ASSERT_EQ( expectedNumWorkersAvail,
                   it->second.NumWorkersAvailable() ) << "failure at line " << line;
        ASSERT_EQ( expectedNumRequests,
                   it->second.NumRequestsPending() )
            << "failure at line " << line << "[" << requestType << "]";
        STATS_ASSERT_EQ2(
            expectedNumWorkersRegistered,
            it->second.stats[ RequestTypeStat::WorkersRegistered ],
            "failure at line " << line );

        if( expectedNextAvailWorker )
        {
            ASSERT_EQ( expectedNextAvailWorker,
                       it->second.GetNextAvailableWorker() ) << "failure at line " << line;
        }
    }

    void checkBrokerNotInProgressRequest(
        unsigned int line,
        const Broker& broker,
        RequestType requestType,
        RequestId rid )
    {
        auto it = broker.requestTypeContextLookup.find( requestType );
        ASSERT_TRUE( it != broker.requestTypeContextLookup.end() )  << "failure at line " << line;
        ASSERT_TRUE( ( nullptr == it->second.GetRequestState( rid ) ) ||
                       !( it->second.GetRequestState( rid )->IsInProgress() ) )
            << "failure at line " << line << "[" << requestType << "|" << rid << "]";
    }

    void checkBrokerInProgressRequest(
        unsigned int line,
        const Broker& broker,
        RequestType requestType,
        RequestId rid,
        WorkerInfo* workerInfo )
    {
        auto it = broker.requestTypeContextLookup.find( requestType );
        auto it2 = workerInfo->GetInProgressRequests().find( requestType ); // id -> state
        ASSERT_TRUE( it != broker.requestTypeContextLookup.end() );
        ASSERT_TRUE( nullptr != it->second.GetRequestState( rid ) )
            << "failure at line " << line << "[" << requestType << "|" << rid << "]";
        ASSERT_TRUE( it->second.GetRequestState( rid )->IsInProgress() )
            << "failure at line " << line << "[" << requestType << "|" << rid << "]";
        ASSERT_EQ( workerInfo, it->second.GetRequestState( rid )->GetAssignedWorker() )
            << "failure at line " << line << "[" << requestType << "|" << rid << "]";
        ASSERT_TRUE( it2 != workerInfo->GetInProgressRequests().end() )
            << "failure at line " << line;

        auto it3 = it2->second.find( rid );    // state
        ASSERT_TRUE( it3 != it2->second.end() ) << "failure at line " << line;
    }


    void checkBrokerRequestTypeContext(
        unsigned int line,
        const Broker& broker,
        RequestType requestType,
        unsigned int expectedNumWorkersTotal,
        unsigned int expectedNumWorkersRegistered,
        unsigned int expectedNumWorkersAvail,
        unsigned int expectedNumRequests )
    {
        checkBrokerRequestTypeContext(
            line, broker, requestType,
            expectedNumWorkersTotal, expectedNumWorkersRegistered,
            expectedNumWorkersAvail, expectedNumRequests, nullptr );
    }

    void addRequest(
        unsigned int line,
        Broker& broker,
        const vector< const char* >& env,
        const LHCVersionFlags& vfs,
        RequestType rt, 
        RequestId rid,
        const vector< const char* >& data )
    {
        ZMQFrameHandler frameHandler;
        auto it = broker.requestTypeContextLookup.find( rt );
        ASSERT_TRUE( it != broker.requestTypeContextLookup.end() ) << "failure at line " << line;

        ZMQMessage* clientRequest = createClientNewRequest(
            frameHandler, env, true, &vfs, &rt, &rid, data );

        it->second.AddPendingRequest( rid, vfs, &clientRequest );
    }

 
    TEST_F( TestBroker, TestUpdateContextsForNewWorker )
    {
        RequestType rt1 = 1,
                    rt2 = 2,
                    rt3 = 3,
                    rt4 = 400,
                    rt5 = 5000,
                    rt6 = 401,
                    rt7 = 4999,
                    rt8 = 5001,
                    rt9 = 6,
                    rt10 = 7,
                    rt11 = 8,
                    rt20 = 900,
                    rt21 = 901,
                    rt22 = 902,
                    rt23 = 903;

        vector< RequestType > rts1
        {
            rt1, rt2, rt3
        };  // 3 types

        vector< RequestType > rts2
        {
            rt2, rt3, rt4, rt5
        };  // 2 new types

        vector< RequestType > rts3
        {
            rt6, rt5, rt7, rt3, rt8, rt9, rt10, rt11
        };  // 6 new types

        vector< RequestType > rts4
        {
            rt20, rt21, rt22, rt23
        };

        vector< const char* > env1{ "env1a", "env1b" };

        vector< const char* > env2{ "env2a" };

        vector< const char* > env20{ "env20a" };
        vector< const char* > env21{ "env21a" };
        vector< const char* > env22{ "env22a" };

        vector< const char* > env30{ "env30a" };
        vector< const char* > env31{ "env31a" };

        vector< const char* > data1{ "data1a", "data1b" };

        vector< const char* > data2{ "data2a", "data2b" };

        vector< const char* > data20{ "data20a" };
        vector< const char* > data21{ "data21a" };
        vector< const char* > data22{ "data22a" };

        vector< const char* > data30{ "data30a" };
        vector< const char* > data31{ "data31a" };

        RequestId rid1( 1000000000 );
        RequestId rid2( 3 );
        RequestId rid5( 98 );
        RequestId rid6( 980 );

        WorkerInfo* workerInfo1( nullptr );
        WorkerInfo* workerInfo2( nullptr );
        WorkerInfo* workerInfo3( nullptr );
        WorkerInfo* workerInfo4a( nullptr );
        WorkerInfo* workerInfo4b( nullptr );
        WorkerInfo* workerInfo5( nullptr );

        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        unique_ptr< Broker > lpBroker( createBroker( socketFactory, senderReceiver ) );
        ZMQFrameHandler frameHandler;

        senderReceiver.sendReturnValues = { 0, 0, 0, 1, 0, 0, 1 };

        // Broker has two primary containers:
        //  1) workerInfoLookup
        //  2) requestTypeContextLookup
        // A WorkerInfo will be inserted into and owned by the workerInfoLookup and pointed to by
        // requestTypeContextLookup

        // Upon creating a Broker, I expect that both containers will be empty

        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ( 0, lpBroker->requestTypeContextLookup.size() );
        {
            auto lpWorkerInfo1 = createWorkerInfo( frameHandler, "worker1", 1, rts1 );
            auto lpWorkerInfo2 = createWorkerInfo( frameHandler, "worker2", 1, rts2 );
            auto lpWorkerInfo3 = createWorkerInfo( frameHandler, "worker3", 2, rts3 );
            auto lpWorkerInfo4a = createWorkerInfo( frameHandler, "worker4a", 1, rts4 );
            auto lpWorkerInfo4b = createWorkerInfo( frameHandler, "worker4b", 4, rts4 );
            auto lpWorkerInfo5 = createWorkerInfo( frameHandler, "worker5", 1, rts3 );

            workerInfo1 = lpWorkerInfo1.get();
            workerInfo2 = lpWorkerInfo2.get();
            workerInfo3 = lpWorkerInfo3.get();
            workerInfo4a = lpWorkerInfo4a.get();
            workerInfo4b = lpWorkerInfo4b.get();
            workerInfo5 = lpWorkerInfo5.get();

            // add workerInfo1 without any additional workers present
            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
            ASSERT_EQ( 1, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo1,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo1->GetIdentity() ) );

            lpBroker->updateContextsForNewWorker( workerInfo1 );
            ASSERT_EQ( 3, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt1,
                1, 1, 1, 0, workerInfo1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt2,
                1, 1, 1, 0, workerInfo1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt3,
                1, 1, 1, 0, workerInfo1 ) );

            // add workerInfo2 with workerInfo1 still there
            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
            ASSERT_EQ( 2, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo2,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo2->GetIdentity() ) );

            lpBroker->updateContextsForNewWorker( workerInfo2 );
            ASSERT_EQ( 5, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt1,
                1, 1, 1, 0, workerInfo1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt2,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt3,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt4,
                1, 1, 1, 0, workerInfo2 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt5,
                1, 1, 1, 0, workerInfo2 ) );

            // no requests, nothing should be sent
            ASSERT_EQ( 0, senderReceiver.sentMessages.size() );

            // lets add requests for rt3 and rt5
            addRequest( __LINE__, 
                        *lpBroker, env1, MessageFlag::Asynchronous, rt3, rid1, data1 );
            addRequest( __LINE__, 
                        *lpBroker, env2, MessageFlag::None, rt5, rid2, data2 );

            // add workerInfo3 with workerInfo1 and workerInfo2 present
            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
            ASSERT_EQ( 3, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo3,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo3->GetIdentity() ) );

            lpBroker->updateContextsForNewWorker( workerInfo3 );
            ASSERT_EQ( 11, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 11, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt1,
                1, 1, 1, 0, workerInfo1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt2,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt3,
                3, 3, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt4,
                1, 1, 1, 0, workerInfo2 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt5,
                2, 2, 1, 0, workerInfo2 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt6,
                1, 1, 0, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt7,
                1, 1, 0, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt8,
                1, 1, 0, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt9,
                1, 1, 0, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt10,
                1, 1, 0, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt11,
                1, 1, 0, 0 ) );

            ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt3,
                rid1,
                workerInfo3 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt5,
                rid2,
                workerInfo3 ) );

            ASSERT_EQ( 2, senderReceiver.sentMessages.size() );

            ASSERT_NO_FATAL_FAILURE( checkForwardedClientRequest(
                __LINE__,
                frameHandler,
                senderReceiver.sentMessages[ 1 ].second,
                { "worker3" },
                env1,
                MessageFlag::Asynchronous,
                rt3,
                rid1,
                data1 ) );

            ASSERT_NO_FATAL_FAILURE( checkForwardedClientRequest(
                __LINE__,
                frameHandler,
                senderReceiver.sentMessages[ 0 ].second,
                { "worker3" },
                env2,
                MessageFlag::None,
                rt5,
                rid2,
                data2 ) );

            // failing to forward existing request does not prevent a second sweep
            // of requests into the worker
            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4a ) );
            ASSERT_EQ( 4, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo4a,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo4a->GetIdentity() ) );
            lpBroker->updateContextsForNewWorker( workerInfo4a );
            ASSERT_EQ( 15, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 15, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

            addRequest( __LINE__, 
                        *lpBroker, env20, MessageFlag::None, rt20, rid1, data20 );
            addRequest( __LINE__, 
                        *lpBroker, env21, MessageFlag::None, rt21, rid1, data21 );
            addRequest( __LINE__, 
                        *lpBroker, env21, MessageFlag::None, rt21, rid2, data21 );

            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4b ) );
            ASSERT_EQ( 5, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo4b,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo4b->GetIdentity() ) );

            lpBroker->updateContextsForNewWorker( workerInfo4b );
            ASSERT_EQ( 15, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 15, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
            STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );

            // four sends: 1) for rt20/rid1, 2) fail for rt21/rid1 3,4) rt21/rid1, rt21,rid2 on
            //                                                          second pass
            ASSERT_EQ( 6, senderReceiver.sentMessages.size() );
            ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt20,
                rid1,
                workerInfo4b ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt21,
                rid1,
                workerInfo4b ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt21,
                rid2,
                workerInfo4b ) );

            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt20,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt21,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt22,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt23,
                2, 2, 2, 0 ) );

            // test that failure to send does not prevent the worker from being registered
            // lets add requests for rt3 and rt5 again
            addRequest( __LINE__, 
                        *lpBroker, env1, MessageFlag::Asynchronous, rt3, rid5, data1 );
            addRequest( __LINE__, 
                        *lpBroker, env2, MessageFlag::None, rt5, rid6, data2 );

            lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo5 ) );
            ASSERT_EQ( 6, lpBroker->workerInfoLookup.UnderlyingMap().size() );
            ASSERT_EQ(
                workerInfo5,
                lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfo5->GetIdentity() ) );

            lpBroker->updateContextsForNewWorker( workerInfo5 );

            ASSERT_EQ( 15, lpBroker->requestTypeContextLookup.size() );
            STATS_ASSERT_EQ( 15, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
            STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt1,
                1, 1, 1, 0, workerInfo1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt2,
                2, 2, 2, 0 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt3,
                4, 4, 3, 1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt4,
                1, 1, 1, 0, workerInfo2 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt5,
                3, 3, 2, 1 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt6,
                2, 2, 1, 0, workerInfo5 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt7,
                2, 2, 1, 0, workerInfo5 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt8,
                2, 2, 1, 0, workerInfo5 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt9,
                2, 2, 1, 0, workerInfo5 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt10,
                2, 2, 1, 0, workerInfo5 ) );
            ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rt11,
                2, 2, 1, 0, workerInfo5 ) );

            ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        }
    }

    void checkWorkerHeartbeat(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* heartbeatMsg,
        WorkerInfo* workerInfo,
        std::time_t sentAt,
        Delay newSendDelay )
    {
        // worker info + null + message type + sent at + delay
        ASSERT_EQ( 5, zmsg_size( heartbeatMsg ) ) << "failure at line " << line;
        ASSERT_EQ( frameHandler.get_frame_value< string >(
                       const_cast< ZMQFrame* >( workerInfo->GetIdentity() ) ),
                   frameHandler.get_frame_value< string >( zmsg_first( heartbeatMsg ) ) )
            << "failure at line " << line;
        ASSERT_EQ( 0, zframe_size( zmsg_next( heartbeatMsg ) ) )
            << "failure at line " << line;
        ASSERT_EQ( ClusterMessage::BrokerHeartbeat,
                   frameHandler.get_frame_value< ClusterMessage >( zmsg_next( heartbeatMsg ) ) )
            << "failure at line " << line;
        ASSERT_EQ( sentAt, 
                   frameHandler.get_frame_value< std::time_t >( zmsg_next( heartbeatMsg ) ) )
            << "failure at line " << line;
        ASSERT_EQ( newSendDelay.count(),
                   frameHandler.get_frame_value< Delay::rep >( zmsg_next( heartbeatMsg ) ) )
            << "failure at line " << line;
    }

    TEST_F( TestBroker, TestSendWorkerHeartbeat )
    {
        // Will want to check
        //  1)  that the message is sent to specified worker's identity
        //  2)  that the message is formatted as one would expect
        //      |_  feed through a test Worker to make sure that no errors are raised
        //          |_  chop off the first two frames (identity and null delim)
        //  3)  that the worker info's broker last active is updated to the current time

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfoHB1 = createWorkerInfo( frameHandler, "workerHB1", 1, { 1, 2 ,3 } );
        auto workerInfoHB1 = lpWorkerInfoHB1.get();
        int ret = 0;

        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfoHB1 ) );
        ASSERT_EQ( 1, lpBroker->workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ(
            workerInfoHB1,
            lpBroker->workerInfoLookup.LookupWorkerInfo( workerInfoHB1->GetIdentity() ) );
        lpBroker->updateContextsForNewWorker( workerInfoHB1 );

        senderReceiver.sendReturnValues = { 0, 1 };

        lpBroker->currentTime = 12345;
        ret = lpBroker->sendWorkerHeartbeat( workerInfoHB1 );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        ASSERT_EQ(
            (TestZMQSocket*)( lpBroker->backendMessagingSocket ),
            senderReceiver.sentMessages[ 0 ].first );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSent ] );
        ASSERT_EQ( 12345, workerInfoHB1->GetBrokerLastActive() );
        ASSERT_TRUE( nullptr != senderReceiver.sentMessages[ 0 ].second );
        ASSERT_NO_FATAL_FAILURE( checkWorkerHeartbeat(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 0 ].second,
            workerInfoHB1,
            lpBroker->currentTime,
            lpBroker->selfHeartbeatDelay ) );

        lpBroker->currentTime = 54321;
        ret = lpBroker->sendWorkerHeartbeat( workerInfoHB1 );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
        ASSERT_EQ(
            (TestZMQSocket*)( lpBroker->backendMessagingSocket ),
            senderReceiver.sentMessages[ 1 ].first );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSentFailed ] );
        ASSERT_EQ( 12345, workerInfoHB1->GetBrokerLastActive() );   // unchanged
        ASSERT_EQ( nullptr, senderReceiver.sentMessages[ 1 ].second );
    }


    TEST_F( TestBroker, TestHandleWorkerReady )
    {
        // TestUpdateContextsForNewWorker should do all of the heavy lifting
        // we mostly just want to check that handleWorkerReady is called which is to say:
        //  1.  if worker exists then nothing happens
        //  2.  if worker does not exist then
        //      a.  checks for at least 3 frames or fails with malformed
        //      b.  parses the correct free capaciy
        //      c.  parses the heartbeat send delay
        //      d.  parses the supported request types (including zero)
        //      e.  the worker info is inserted and registered
        //  Specific Ready Msg Cases
        //  1.  new worker info - inserted into broker and all rts registered
        //  2.  duplicate worker info -> nothing happens
        //  3.  new worker with overlapping rts -> inserted into broker and difference
        //      |_  include 0 rt
        //  4.  less than 3 frames -> malformed
        //  5.  negative free capacity - malformed
        //  6.  TODO - bad delay - malformed
        //  7.  null rtype frame - malformed
        //  General Message 
        //  1. Failed to receive - increment FailedWorkerReceives
        //  2. Missing Identity - increment MalformedBackendMessage
        //  3. Missing Null Delimiter - increment MalformedBackendMessage
        //  4. Missing ClusterMessage - increment MalformedBackendMessage
        //  5. Unknown ClusterMessage - increment UnknownWorkerMessage

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );

        ZMQMessage* missingIdentity = nullptr;
        ZMQMessage* missingNullDelim = nullptr;
        ZMQMessage* missingWorkerMessage = nullptr;
        ZMQMessage* unknownWorkerMessage = nullptr;

        ZMQMessage* lessThanThreeFrames = nullptr;
        ZMQMessage* negativeFreeCapacity = nullptr;
        ZMQMessage* nullRequestTypeFrame = nullptr;

        ZMQMessage* newWorkerReadyMsg = nullptr;
        ZMQMessage* dupWorkerReadyMsg = nullptr;
        ZMQMessage* overlappingWorkerReadyMsg = nullptr;
        ZMQMessage* missingVFS = nullptr;

        ClusterMessage workerReady = ClusterMessage::WorkerReady;
        ClusterMessage unknownWorkerMessageVal = (ClusterMessage) -1;
        LHCVersionFlags vfs;
        int freeCapacity = 1;
        int negativeCapacity = -1;
        Delay delay( 3 );
        int ret = 0;

        missingIdentity = createWorkerMessage(
            frameHandler, nullptr, true, &workerReady );
        missingNullDelim = createWorkerMessage(
            frameHandler, "missingNullDelim", false, &workerReady );
        missingWorkerMessage = createWorkerMessage(
            frameHandler, "missingWorkerMessage", true, nullptr );
        unknownWorkerMessage = createWorkerMessage(
            frameHandler, "unknownWorkerMessage", true, &unknownWorkerMessageVal );

        lessThanThreeFrames = createWorkerReady(
            frameHandler, "lessThanThreeFrames", true, &workerReady,
            &vfs,
            &freeCapacity, &delay, false, {} );

        negativeFreeCapacity = createWorkerReady(
            frameHandler, "negativeFreeCapacity", true, &workerReady,
            &vfs,
            &negativeCapacity, &delay, true, {} );

        nullRequestTypeFrame = createWorkerReady(
            frameHandler, "nullRequestTypeFrame", true, &workerReady,
            &vfs,
            &freeCapacity, &delay, true, {} );

        newWorkerReadyMsg = createWorkerReady(
            frameHandler, "newWorkerReadyMsg", true, &workerReady,
            &vfs,
            &freeCapacity, &delay, false, {1, 2, 3} );

        dupWorkerReadyMsg = zmsg_dup( newWorkerReadyMsg );

        overlappingWorkerReadyMsg = createWorkerReady(
            frameHandler, "overlappingWorkerReadyMsg", true, &workerReady,
            &vfs,
            &freeCapacity, &delay, false, {3, 4, 5, 0} );

        missingVFS = createWorkerReady(
            frameHandler, "missingVFS", true, &workerReady,
            nullptr,
            &freeCapacity, &delay, false, {345, 0} );

        senderReceiver.receiveReturnValues = {
            nullptr,
            missingIdentity,
            missingNullDelim,
            missingWorkerMessage,
            unknownWorkerMessage,
            lessThanThreeFrames,
            negativeFreeCapacity,
            nullRequestTypeFrame,
            newWorkerReadyMsg,
            dupWorkerReadyMsg,
            overlappingWorkerReadyMsg,
            missingVFS
        };

        lpBroker->currentTime = 12345;
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        ASSERT_TRUE( 12345 != lpBroker->currentTime );  //time updated
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::FailedWorkerReceives ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedBackendMessages ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::MalformedBackendMessages ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::MalformedBackendMessages ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::UnknownWorkerMessages ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedWorkerReadies ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::MalformedWorkerReadies ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::MalformedWorkerReadies ] );
        ASSERT_EQ( 0, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::WorkerReadies ] );
        ASSERT_EQ( 1, lpBroker->workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ( 3, lpBroker->requestTypeContextLookup.size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::WorkerReadies ] );
        ASSERT_EQ( 1, lpBroker->workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ( 3, lpBroker->requestTypeContextLookup.size() );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            1,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            2,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            3,
            1, 1, 1, 0 ) );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::WorkerReadies ] );
        ASSERT_EQ( 2, lpBroker->workerInfoLookup.UnderlyingMap().size() );
        ASSERT_EQ( 6, lpBroker->requestTypeContextLookup.size() );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::MalformedWorkerReadies ] );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            1,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            2,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            3,
            2, 2, 2, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            4,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            5,
            1, 1, 1, 0 ) );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            0,
            1, 1, 1, 0 ) );
    }

    TEST_F( TestBroker, TestSendWorkerHeartbeats )
    {
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        Delay d1( 1 );
        Delay d2( 2 );
        Delay d4( 4 );
        auto lpWorkerInfoHB1 = createWorkerInfo( frameHandler, "workerHB1", 1, { 1 }, d1, 9 );
        auto workerInfoHB1 = lpWorkerInfoHB1.get();
        auto lpWorkerInfoHB2 = createWorkerInfo( frameHandler, "workerHB2", 1, { 2 }, d1, 10 );
        auto workerInfoHB2 = lpWorkerInfoHB2.get();
        auto lpWorkerInfoHB3 = createWorkerInfo( frameHandler, "workerHB3", 1, { 3 }, d2, 1 );
        auto workerInfoHB3 = lpWorkerInfoHB3.get();
        auto lpWorkerInfoHB4 = createWorkerInfo( frameHandler, "workerHB4", 1, { 1 }, d4, 7 );
        auto workerInfoHB4 = lpWorkerInfoHB4.get();
        int ret = 0;

        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfoHB1 ) );
        lpBroker->updateContextsForNewWorker( workerInfoHB1 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfoHB2 ) );
        lpBroker->updateContextsForNewWorker( workerInfoHB2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfoHB3 ) );
        lpBroker->updateContextsForNewWorker( workerInfoHB3 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfoHB4 ) );
        lpBroker->updateContextsForNewWorker( workerInfoHB4 );

        lpBroker->currentTime = 10;

        senderReceiver.sendReturnValues = { 1, 0 };

        ASSERT_TRUE( workerInfoHB1->IsHeartbeatNeededAsOf( lpBroker->currentTime ) );
        ASSERT_FALSE( workerInfoHB2->IsHeartbeatNeededAsOf( lpBroker->currentTime ) );
        ASSERT_TRUE( workerInfoHB3->IsHeartbeatNeededAsOf( lpBroker->currentTime ) );
        ASSERT_FALSE( workerInfoHB4->IsHeartbeatNeededAsOf( lpBroker->currentTime ) );

        // one send should fail -> stat increase and one of hb1 hb3 should not have last active set
        ret = lpBroker->sendWorkerHeartbeats();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSentFailed ] );
        ASSERT_TRUE( ( workerInfoHB1->GetBrokerLastActive() == 10 ) ^
                     ( workerInfoHB3->GetBrokerLastActive() == 10 ) )
            << "either both failed or both succeeded, would have expected exactly one to fail";
        ASSERT_EQ( 10, workerInfoHB2->GetBrokerLastActive() ); 
        ASSERT_EQ( 7, workerInfoHB4->GetBrokerLastActive() ); 

        lpBroker->currentTime = 12;

        // hb1, hb2, hb4 should send, all succeed

        ret = lpBroker->sendWorkerHeartbeats();
        ASSERT_EQ( ret, 0 );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsSentFailed ] );
        ASSERT_EQ( 12, workerInfoHB1->GetBrokerLastActive() ); 
        ASSERT_EQ( 12, workerInfoHB2->GetBrokerLastActive() ); 
        ASSERT_EQ( 12, workerInfoHB3->GetBrokerLastActive() ); 
        ASSERT_EQ( 12, workerInfoHB4->GetBrokerLastActive() ); 
    }

    TEST_F( TestBroker, TestUpdateContextsForDeadWorker )
    {
        // Want to check that
        //  1. Requests assigned to a dead worker are removed
        //  2. Stats are updated accordingly
        //  3. Other requests are untouched
        // Try with three workers, two overlap
        // Assign several requests (same ids but for different types to each except for overlapping)
        // to each worker and fail each one
        // Check that at update the other workrs and requests are untouched
        // At the end every request should be removed
        //TODO - this will change if we ever add 1) retry 2) expiring request types

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 0,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo( frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3} );
        auto lpWorkerInfo1 = createWorkerInfo( frameHandler, "worker1", 2, {rt0, rt1} );
        auto lpWorkerInfo2 = createWorkerInfo( frameHandler, "worker2", 3, {rt1, rt2} );
        auto lpWorkerInfo3 = createWorkerInfo( frameHandler, "worker3", 3, {rt3} );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        auto workerInfo3 = lpWorkerInfo3.get();

        RequestId rid1 = 1,
                  rid2 = 2,
                  rid3 = 3;

        vector< const char* > env{ "enva", "envb", "envc" };
        vector< const char* > data{ "dataa" };

        // create an easier function to do this rather than relying on the
        // correct behavior of updateContextsForNewWorker
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

#define checkDeaths( line, rt0Deaths, rt1Deaths, rt2Deaths, rt3Deaths ) \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt0, rt0Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt1, rt1Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt2, rt2Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt3, rt3Deaths )

        // for worker 1
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous,
                    rt0, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid1,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        // for worker 2
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );

        // for worker 3
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
        lpBroker->updateContextsForNewWorker( workerInfo3 );

        // should not be touched
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt0, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid3,
                    data );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt0,
            2, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt1,
            3, 3, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt2,
            2, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt3,
            2, 2, 1, 1, workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1,
            workerInfo1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1,
            workerInfo1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid1,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1,
            workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2,
            workerInfo3 ) );

        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 0, 0, 0, 0 ) );

        lpBroker->updateContextsForDeadWorker( workerInfo0 );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt0,
            1, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt1,
            2, 3, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt2,
            1, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt3,
            1, 2, 1, 1, workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1,
            workerInfo1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1,
            workerInfo1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid1,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1,
            workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2,
            workerInfo3 ) );

        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 1, 1, 1, 1 ) );

        lpBroker->updateContextsForDeadWorker( workerInfo1 );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt0,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt1,
            1, 3, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt2,
            1, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt3,
            1, 2, 1, 1, workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid1,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid2,
            workerInfo2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1,
            workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2,
            workerInfo3 ) );


        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 2, 2, 1, 1 ) );

        lpBroker->updateContextsForDeadWorker( workerInfo2 );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt0,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt1,
            0, 3, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt2,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt3,
            1, 2, 1, 1, workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1,
            workerInfo3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2,
            workerInfo3 ) );

        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 2, 3, 2, 1 ) );

        lpBroker->updateContextsForDeadWorker( workerInfo3 );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt0,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt1,
            0, 3, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt2,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext(
            __LINE__,
            *lpBroker,
            rt3,
            0, 2, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt2,
            rid2 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2 ) );

        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 2, 3, 2, 2 ) );
    }

    TEST_F( TestBroker, TestCheckWorkersPulse )
    {
        // Just want to check that pulse is checked correctly
        // and that rt workersdied is updated and that the worker is removed
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 0,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3;
        time_t currentTime = 80;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo(
            frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3}, 81 );
        auto lpWorkerInfo1 = createWorkerInfo(
            frameHandler, "worker1", 2, {rt0, rt1}, 70 );
        auto lpWorkerInfo2 = createWorkerInfo(
            frameHandler, "worker2", 3, {rt1, rt2}, 80 );
        auto lpWorkerInfo3 = createWorkerInfo(
            frameHandler, "worker3", 3, {rt3}, 79 );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        auto workerInfo3 = lpWorkerInfo3.get();

        RequestId rid1 = 1,
                  rid2 = 2,
                  rid3 = 3;

        vector< const char* > env{ "enva", "envb", "envc" };
        vector< const char* > data{ "dataa" };

        lpBroker->currentTime = currentTime;
        lpBroker->workerDeathDelay = Delay( 1 );
        // expecting two deaths: 70 + 1 <= 80 and 79+1 <= 80
        // workers 1 and 3

        // create an easier function to do this rather than relying on the
        // correct behavior of updateContextsForNewWorker
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

#define checkDeaths( line, rt0Deaths, rt1Deaths, rt2Deaths, rt3Deaths ) \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt0, rt0Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt1, rt1Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt2, rt2Deaths ); \
        checkBrokerRequestTypeContextDeaths( line, *lpBroker, rt3, rt3Deaths )

        // for worker 1
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt0, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid1,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        // for worker 2
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );

        // for worker 3
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
        lpBroker->updateContextsForNewWorker( workerInfo3 );

        // should not be touched
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt0, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt1, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt2, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env,
                    MessageFlag::Asynchronous, 
                    rt3, rid3,
                    data );

        lpBroker->checkWorkersPulse();

        ASSERT_NO_FATAL_FAILURE( checkDeaths( __LINE__, 1, 1, 0, 1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt0,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2 ) );

        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::WorkerDeaths ] );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestsFailedDueToWorkerDeath ] );
        ASSERT_EQ( 2, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        ZMQMessage* tmpMsg = zmsg_new();
        frameHandler.append_message_value( tmpMsg, string( "worker1" ) );
        frameHandler.append_message_value( tmpMsg, string( "worker3" ) );
        ASSERT_EQ( nullptr,
                   lpBroker->workerInfoLookup.LookupWorkerInfo( zmsg_first( tmpMsg ) ) );
        ASSERT_EQ( nullptr,
                   lpBroker->workerInfoLookup.LookupWorkerInfo( zmsg_next( tmpMsg ) ) );
        zmsg_destroy( &tmpMsg );
    }

    TEST_F( TestBroker, TestHandleWorkerHeartbeat )
    {
        //  Effects:
        //      1)  BrokerStat::WorkerHeartbeatsReceived
        //      2)  workerInfo->UpdateWithHeartbeat(currentTime, sentAt, newHeartbeatSendDelay)
        //      TODO - if worker not found then send back a message requesting a register msg
        //  Format:
        //      0)  Address Frames, Null Frame
        //      1)  ClusterMessage::WorkerHeartbeat
        //      2)  Sent At
        //      3)  New Heartbeat Send Delay
        //  Cases
        //      1)  No sentAt
        //      2)  Bad sentAt (negative?)
        //      3)  No newHeartbeatSendDelay
        //      4)  Bad newHeartbeatSendDelay (negative, too large?)
        //      5)  unknown worker
        //      6)  known worker1
        //      7)  known worker2
        std::time_t goodSentAt( 3 );
        std::time_t badSentAt( -3 );
        Delay goodDelay( 10 );
        Delay badDelay( -10 );
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 0,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3;
        time_t currentTime = 800;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo(
            frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3}, 81 );
        auto lpWorkerInfo1 = createWorkerInfo(
            frameHandler, "worker1", 2, {rt0, rt1}, 70 );
        auto lpWorkerInfo2 = createWorkerInfo(
            frameHandler, "worker2", 3, {rt1, rt2}, 80 );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        Delay workerInfo0Delay( workerInfo0->GetHeartbeatSentDelay() );
        Delay workerInfo1Delay( workerInfo1->GetHeartbeatSentDelay() );
        Delay workerInfo2Delay( workerInfo2->GetHeartbeatSentDelay() );
        ZMQMessage* noSentAtMsg( nullptr );
        ZMQMessage* badSentAtMsg( nullptr );
        ZMQMessage* noDelayMsg( nullptr );
        ZMQMessage* badDelayMsg( nullptr );
        ZMQMessage* unknownWorkerMsg( nullptr );
        ZMQMessage* worker1HBMsg( nullptr );
        ZMQMessage* worker2HBMsg( nullptr );
        int ret = 0;

        lpBroker->currentTime = currentTime;

        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
        ASSERT_EQ( 3, lpBroker->workerInfoLookup.UnderlyingMap().size() );

        noSentAtMsg = createWorkerHeartbeat( frameHandler, "worker0", true,
                                          nullptr, &goodDelay );
        badSentAtMsg = createWorkerHeartbeat( frameHandler, "worker0", true,
                                           &badSentAt, &goodDelay );
        noDelayMsg = createWorkerHeartbeat( frameHandler, "worker0", true,
                                         &goodSentAt, nullptr );
        badDelayMsg = createWorkerHeartbeat( frameHandler, "worker0", true,
                                          &goodSentAt, &badDelay );
        unknownWorkerMsg = createWorkerHeartbeat( frameHandler, "workerX", true,
                                               &goodSentAt, &goodDelay );
        worker1HBMsg = createWorkerHeartbeat( frameHandler, "worker1", true,
                                           &goodSentAt, &goodDelay );
        worker2HBMsg = createWorkerHeartbeat( frameHandler, "worker2", true,
                                           &goodSentAt, &goodDelay );

        ret = lpBroker->handleBackendMessage( &noSentAtMsg );
        ASSERT_EQ( 2, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 70, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo1Delay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &badSentAtMsg );
        ASSERT_EQ( 2, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 70, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo1Delay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &noDelayMsg );
        ASSERT_EQ( 3, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 70, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo1Delay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &badDelayMsg );
        ASSERT_EQ( 3, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 70, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo1Delay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &unknownWorkerMsg );
        ASSERT_EQ( 1, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 70, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo1Delay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &worker1HBMsg );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 800, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 80, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( goodDelay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( workerInfo2Delay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ret = lpBroker->handleBackendMessage( &worker2HBMsg );
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 81, workerInfo0->GetWorkerLastActive() );
        ASSERT_EQ( 800, workerInfo1->GetWorkerLastActive() );
        ASSERT_EQ( 800, workerInfo2->GetWorkerLastActive() );
        ASSERT_EQ( workerInfo0Delay, workerInfo0->GetHeartbeatSentDelay() );
        ASSERT_EQ( goodDelay, workerInfo1->GetHeartbeatSentDelay() );
        ASSERT_EQ( goodDelay, workerInfo2->GetHeartbeatSentDelay() );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::WorkerHeartbeatsReceived ] );

        ASSERT_EQ( 0, senderReceiver.sentMessages.size() );
    }

    TEST_F( TestBroker, TestHandleWorkerReceipt )
    {
        //  Effects
        //  1)  BrokerStat::WorkerReceiptsReceived++
        //  2)  on Send success BrokerStat::WorkerReceiptsSent++
        //  3)  on Send failed BrokerStat::WorkerReceiptsSentFailed++
        //  4)  Send ret returned to caller
        ZMQMessage* receiptSucceed( nullptr );
        ZMQMessage* receiptSucceed2( nullptr );
        ZMQMessage* receiptFailed( nullptr );
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        vector< const char* > env1{ "env1a", "env1b" };
        vector< const char* > env2{ "env2a" };
        auto lpWorkerInfo0 = createWorkerInfo(
            frameHandler, "worker0", 0, {1}, 81 );
        auto lpWorkerInfo1 = createWorkerInfo(
            frameHandler, "worker1", 2, {2}, 70 );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        int ret = 0;
 
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        workerInfo0->UpdateWorkerLastActive( 1 );
        workerInfo1->UpdateWorkerLastActive( 1 );

        receiptSucceed = createWorkerRequestReceipt(
            frameHandler,
            1,
            10,
            env1,
            "worker0" );

        receiptFailed = createWorkerRequestReceipt(
            frameHandler,
            2,
            20,
            env1,
            "worker1" );

        receiptSucceed2 = createWorkerRequestReceipt(
            frameHandler,
            3,
            30,
            env1,
            "workerUnknown" );

        senderReceiver.receiveReturnValues =
            { receiptSucceed, receiptFailed, receiptSucceed2, nullptr };
        senderReceiver.sendReturnValues = { 0, 1, 0 };

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerReceiptsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerReceiptsSent ] );
        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::WorkerReceiptsSentFailed ] );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        ASSERT_EQ( ( TestZMQSocket* ) lpBroker->frontendMessagingSocket,
                   senderReceiver.sentMessages[ 0 ].first );
        ASSERT_NO_FATAL_FAILURE(
            checkReceipt(
                frameHandler,
                senderReceiver.sentMessages[ 0 ].second,
                1, 10, env1, true ) );
        ASSERT_TRUE( workerInfo0->GetWorkerLastActive() > 1 );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::WorkerReceiptsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerReceiptsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerReceiptsSentFailed ] );
        ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
        ASSERT_TRUE( workerInfo1->GetWorkerLastActive() > 1 );

        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::WorkerReceiptsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::WorkerReceiptsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerReceiptsSentFailed ] );
        ASSERT_EQ( 3, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE(
            checkReceipt(
                frameHandler,
                senderReceiver.sentMessages[ 2 ].second,
                3, 30, env1, true ) );
    }

    TEST_F( TestBroker, TestHandleClientHeartbeat )
    {
        //  Effects
        //  1)  BrokerStat::ClientHeartbeatsReceived++
        //  2)  on Send success BrokerStat::ClientHeartbeatsSent++
        //  3)  on Send failed BrokerStat::ClientHeartbeatsSentFailed++
        //  4)  Send ret returned to caller
        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        vector< const char* > env1{ "env1a", "env1b" };
        vector< const char* > env2{ "env2a" };
        ZMQMessage* clientHeartbeatSucceed( nullptr );
        ZMQMessage* clientHeartbeatFailed( nullptr );
        int ret = 0;

        clientHeartbeatSucceed = createClientHeartbeat( frameHandler, env1 );
        clientHeartbeatFailed = createClientHeartbeat( frameHandler, env2 );

        senderReceiver.receiveReturnValues =
            { clientHeartbeatSucceed, clientHeartbeatFailed, nullptr };
        senderReceiver.sendReturnValues = { 0, 1, 0 };

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientHeartbeatsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientHeartbeatsSent ] );
        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::ClientHeartbeatsSentFailed ] );
        ASSERT_EQ( 1, senderReceiver.sentMessages.size() );
        ASSERT_EQ( ( TestZMQSocket* ) lpBroker->frontendMessagingSocket,
                   senderReceiver.sentMessages[ 0 ].first );
        // everthing pretty much uses the same heartbeat format, not checking it again

        lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientHeartbeatsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientHeartbeatsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientHeartbeatsSentFailed ] );
        ASSERT_EQ( 2, senderReceiver.sentMessages.size() );
    }

    TEST_F( TestBroker, TestHandleWorkerFinishedSynchronous )
    {
        //  This is like TestHandleWorkerReady except:
        //  1)  The specified request type/id should be removed as in progress
        //  2)  New request types are not added

        //  Effects
        //  1)  remove the request from inprogress from the worker and request type context
        //  2)  send the response to client
        //  3)  flush outstanding requests to worker
        //  4)  if no outsta=nding requests then increase capcity by 1

        //  Stats
        //  1)  BrokerStat:SyncRequestsCompleted
        //  2)  BrokerStat::SyncResponsesSent
        //  3)  BrokerStat::SyncResponsesSentFailed
        //  4)  BrokerStat::ResponsesFromUnknownWorkers
        //  5)  RequestTypeStat::RequestsCompleted
        //  6)  BrokerStat::ResponsesForUnknownReqTypes

        //  Cases
        //  1)  Malformed Response
        //      a)  no client env
        //      b)  no broker message
        //      c)  no request type
        //      d)  no request id
        //  2)  Unknown Worker
        //  3)  Unknown Respnse Request Type
        //  4)  There are pending requests
        //      a)  multiple capacity
        //      b)  one capacity
        //  5)  There are no pending requests
        //  6)  Send to client fails
        //  7)  Unknown request id ?

        ZMQMessage* missingClientEnvMsg( nullptr );
        ZMQMessage* missingClientEnvDelimMsg( nullptr );
        ZMQMessage* noBrokerMessageMsg( nullptr );
        ZMQMessage* noRequestTypeMsg( nullptr );
        ZMQMessage* noRequestIdMsg( nullptr );
        ZMQMessage* unknownWorkerMsg( nullptr );
        ZMQMessage* unknownRequestTypeMsg( nullptr ); // should increase capacity, be sent
        ZMQMessage* finishedWithPlus1CapMsg( nullptr ); // worker2 should pick up 2 rt2 requests
        ZMQMessage* finishedWith1CapMsg( nullptr ); // worker1 should pick up 1 rt0 request
        ZMQMessage* finishedWithNoPendReqMsg( nullptr ); // worker3 should pick up nothing
        ZMQMessage* sendToClientFailsMsg( nullptr );  // should not affect state
        ZMQMessage* unknownRequestIdMsg( nullptr ); // should not have an affect on anything

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 10,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3,
                    rtUnknown = 999;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo( frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3} );
        auto lpWorkerInfo1 = createWorkerInfo( frameHandler, "worker1", 2, {rt0, rt1} );
        auto lpWorkerInfo2 = createWorkerInfo( frameHandler, "worker2", 3, {rt2, rt1} );
        auto lpWorkerInfo3 = createWorkerInfo( frameHandler, "worker3", 3, {rt3} );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        auto workerInfo3 = lpWorkerInfo3.get();

        RequestId rid1 = 1,
                  rid2 = 2,
                  rid3 = 3,
                  ridUnknown = 0;

        LHCVersionFlags vfs0( 999, 888, MessageFlag::None );
        LHCVersionFlags vfs1( MessageFlag::ReceiptOnReceive );
        LHCVersionFlags vfs2( 999, 888, MessageFlag::NotifyOnRedirect );
        LHCVersionFlags vfs3( MessageFlag::ReceiptOnReceive | MessageFlag::AllowRedirect );
        LHCVersionFlags vfsUnknown;

        ClusterMessage brokerMessage( ClusterMessage::BrokerResponse );
        ClusterMessage workerMessage( ClusterMessage::WorkerFinished );

        vector< const char* > emptyEnv{};
        vector< const char* > env{ "enva", "envb", "envc" };
        vector< const char* > env2{ "env2a", "env2b", "env2c" };
        vector< const char* > data{ "dataa" };
        vector< const char* > data2{ "data2a" };

        int ret = 0;

        // create an easier function to do this rather than relying on the
        // correct behavior of updateContextsForNewWorker
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

        // for worker 1
        addRequest( __LINE__,  *lpBroker, env,
                    vfs1, 
                    rt0, rid1,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env,
                    vfs1, 
                    rt1, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        // for worker 2
        addRequest( __LINE__,  *lpBroker, env,
                    vfs2, 
                    rt1, rid2,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs1, 
                    rt2, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );

        // for worker 3
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs1, 
                    rt3, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2, 
                    rt3, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
        lpBroker->updateContextsForNewWorker( workerInfo3 );

        // extra in progress requests
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2, 
                    rt0, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs3, 
                    rt1, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2, 
                    rt2, rid2,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs3, 
                    rt2, rid3,
                    data );

#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 1, 2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        missingClientEnvMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            emptyEnv, true, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        missingClientEnvDelimMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, false, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        noBrokerMessageMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, nullptr, &rt0, &rid1, data, &vfs1 );

        noRequestTypeMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, nullptr, &rid1, data, &vfs1 );

        noRequestIdMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, &rt0, nullptr, data, &vfs0 );

        unknownWorkerMsg = createWorkerRequestCompleted(
            frameHandler, "whodat", true, &workerMessage,
            env, true, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        // worker0 should pick up rt0/rid2
        unknownRequestTypeMsg = createWorkerRequestCompleted(
            frameHandler, "worker0", true, &workerMessage,
            env, true, &brokerMessage, &rtUnknown, &rid1, data, &vfs1 );

        // worker2 should pick up rt1/rid3 rt2/rid2 requests
        finishedWithPlus1CapMsg = createWorkerRequestCompleted(
            frameHandler, "worker2", true, &workerMessage,
            env, true, &brokerMessage, &rt1, &rid2, data, &vfs2 );

        // worker1 should pick up 1 rt0 request
        finishedWith1CapMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, &rt1, &rid1, data, &vfs1 );

        // worker3 should pick up nothing
        finishedWithNoPendReqMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &rid1, data, &vfs1 );

        // should not affect state
        sendToClientFailsMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &rid2, data, &vfs2 );

        // should not have an affect on anything
        unknownRequestIdMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &ridUnknown, data, &vfsUnknown );

        senderReceiver.receiveReturnValues = {
            missingClientEnvMsg,        // 1
            missingClientEnvDelimMsg,   // 2
            noBrokerMessageMsg,         // 3
            noRequestTypeMsg,           // 4
            noRequestIdMsg,             // 5
            unknownWorkerMsg,           // 6
            unknownRequestTypeMsg,      // 7
            finishedWithPlus1CapMsg,    // 8
            finishedWith1CapMsg,        // 9
            finishedWithNoPendReqMsg,   // 10
            sendToClientFailsMsg,       // 11
            unknownRequestIdMsg,        // 12
            nullptr
        };

        senderReceiver.sendReturnValues = {
            0
        };

        // 1 - no client env -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 2 - no client env delimiter -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 3 - no broker message -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 4 - no request type -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 5 - no request id -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::SyncResponsesSent ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        // 6 - unknown worker
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 7, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        // 7 -> worker0 should pick up rt0/rid2
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() ); // one msg sent for client response
#undef checkBrokerState                                     // and one sent for worker request
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 1, 2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 )
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_EQ( (TestZMQSocket*) lpBroker->frontendMessagingSocket,
                   senderReceiver.sentMessages[ 7 ].first );
        ASSERT_NO_FATAL_FAILURE( checkForwardedWorkerResponse(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 7 ].second,
            env,
            rtUnknown,
            rid1,
            data,
            vfs1 ) );
        ASSERT_EQ( (TestZMQSocket*) lpBroker->backendMessagingSocket,
                   senderReceiver.sentMessages[ 8 ].first );
        ASSERT_NO_FATAL_FAILURE( checkForwardedClientRequest(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 8 ].second,
            { "worker0" },
            env2,
            vfs2,
            rt0,
            rid2,
            data ) );

        // 8 finshed request with >1 capacity afterwards -> worker2 worker2 should 
        //                                                  pick up rt1/rid3 rt2/rid2 requests
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 12, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2 ) );

        // worker1 should not pick up anything, free capacity should be 1 and should
        // be available for both rt0 and rt1 requests
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 13, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );

        // worker3 should pick up nothing
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 14, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1 ) );

        // TODO - richer testing API - i.e. senderReceiver.nextReturnValue( .. ) ..
        //        instead of this crap
        senderReceiver.sendReturnValues = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,                      // fail one response send
            0
        };

        // should not affect state
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 15, senderReceiver.sentMessages.size() );
        ASSERT_EQ( nullptr, senderReceiver.sentMessages[ 14 ].second );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2 ) );

        // should not have an affect on anything
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 16, senderReceiver.sentMessages.size() );    // response still goes through
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );


        STATS_ASSERT_EQ( 9, lpBroker->stats[ BrokerStat::SyncRequestsCompleted ] );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::SyncResponsesSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::SyncResponsesSentFailed ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ResponsesFromUnknownWorkers ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ResponsesForUnknownReqTypes ] );

        ASSERT_EQ( 0, workerInfo0->FreeCapacity() );
        ASSERT_EQ( 1, workerInfo1->FreeCapacity() );
        ASSERT_EQ( 0, workerInfo2->FreeCapacity() );
        ASSERT_EQ( 4, workerInfo3->FreeCapacity() );
    }

    TEST_F( TestBroker, TestHandleWorkerFinishedAsynchronous )
    {
        // Almost identical to TestHandleWorkerFinishedSynchronous except
        // different stats and nothing should be sent to the client

        ZMQMessage* missingClientEnvMsg( nullptr );
        ZMQMessage* missingClientEnvDelimMsg( nullptr );
        ZMQMessage* noBrokerMessageMsg( nullptr );
        ZMQMessage* noRequestTypeMsg( nullptr );
        ZMQMessage* noRequestIdMsg( nullptr );
        ZMQMessage* unknownWorkerMsg( nullptr );
        ZMQMessage* unknownRequestTypeMsg( nullptr ); // should increase capacity
        ZMQMessage* finishedWithPlus1CapMsg( nullptr ); // worker2 should pick up 2 rt2 requests
        ZMQMessage* finishedWith1CapMsg( nullptr ); // worker1 should pick up 1 rt0 request
        ZMQMessage* finishedWithNoPendReqMsg( nullptr ); // worker3 should pick up nothing
        ZMQMessage* sendToClientFailsMsg( nullptr );  // should not affect state
        ZMQMessage* unknownRequestIdMsg( nullptr ); // should not have an affect on anything
        ZMQMessage* missingVFS( nullptr );

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 10,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3,
                    rtUnknown = 999;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo( frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3} );
        auto lpWorkerInfo1 = createWorkerInfo( frameHandler, "worker1", 2, {rt0, rt1} );
        auto lpWorkerInfo2 = createWorkerInfo( frameHandler, "worker2", 3, {rt2, rt1} );
        auto lpWorkerInfo3 = createWorkerInfo( frameHandler, "worker3", 3, {rt3} );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        auto workerInfo3 = lpWorkerInfo3.get();

        RequestId rid1 = 1,
                  rid2 = 2,
                  rid3 = 3,
                  ridUnknown = 0;

        ClusterMessage brokerMessage( ClusterMessage::BrokerResponse );
        ClusterMessage workerMessage( ClusterMessage::WorkerFinished );

        vector< const char* > emptyEnv{};
        vector< const char* > env{ "enva", "envb", "envc" };
        vector< const char* > env2{ "env2a", "env2b", "env2c" };
        vector< const char* > data{ "dataa" };
        vector< const char* > data2{ "data2a" };

        int ret = 0;

        LHCVersionFlags vfs0( 999, 888, MessageFlag::Asynchronous );
        LHCVersionFlags vfs1( MessageFlag::Asynchronous );
        LHCVersionFlags vfs2( 999, 888, MessageFlag::Asynchronous | MessageFlag::RetryOnDeath );
        LHCVersionFlags vfs3( MessageFlag::Asynchronous | MessageFlag::AllowRedirect );
        LHCVersionFlags vfsUnknown( MessageFlag::Asynchronous );


        // create an easier function to do this rather than relying on the
        // correct behavior of updateContextsForNewWorker
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

        // for worker 1
        addRequest( __LINE__,  *lpBroker, env,
                    vfs1,
                    rt0, rid1,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env,
                    vfs1,
                    rt1, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        // for worker 2
        addRequest( __LINE__,  *lpBroker, env,
                    vfs2,
                    rt1, rid2,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs1,
                    rt2, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );

        // for worker 3
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs1,
                    rt3, rid1,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2,
                    rt3, rid2,
                    data );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
        lpBroker->updateContextsForNewWorker( workerInfo3 );

        // extra in progress requests
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2,
                    rt0, rid2,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs3,
                    rt1, rid3,
                    data );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs2,
                    rt2, rid2,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    vfs3,
                    rt2, rid3,
                    data );

#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 1, 2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        missingClientEnvMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            emptyEnv, true, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        missingClientEnvDelimMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, false, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        noBrokerMessageMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, nullptr, &rt0, &rid1, data, &vfs1 );

        noRequestTypeMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, nullptr, &rid1, data, &vfs1 );

        noRequestIdMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, &rt0, nullptr, data, &vfs1 );

        unknownWorkerMsg = createWorkerRequestCompleted(
            frameHandler, "whodat", true, &workerMessage,
            env, true, &brokerMessage, &rt0, &rid1, data, &vfs1 );

        // worker0 should pick up rt0/rid2
        unknownRequestTypeMsg = createWorkerRequestCompleted(
            frameHandler, "worker0", true, &workerMessage,
            env, true, &brokerMessage, &rtUnknown, &rid1, data, &vfs1 );

        // worker2 should pick up rt1/rid3 rt2/rid2 requests
        finishedWithPlus1CapMsg = createWorkerRequestCompleted(
            frameHandler, "worker2", true, &workerMessage,
            env, true, &brokerMessage, &rt1, &rid2, data, &vfs2 );

        // worker1 should pick up 1 rt0 request
        finishedWith1CapMsg = createWorkerRequestCompleted(
            frameHandler, "worker1", true, &workerMessage,
            env, true, &brokerMessage, &rt1, &rid1, data, &vfs1 );

        // worker3 should pick up nothing
        finishedWithNoPendReqMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &rid1, data, &vfs1 );

        // should not affect state
        sendToClientFailsMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &rid2, data, &vfs2 );

        // should not have an affect on anything
        unknownRequestIdMsg = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &ridUnknown, data, &vfsUnknown );

        missingVFS = createWorkerRequestCompleted(
            frameHandler, "worker3", true, &workerMessage,
            env, true, &brokerMessage, &rt3, &ridUnknown, data, nullptr );

        senderReceiver.receiveReturnValues = {
            missingClientEnvMsg,        // 1
            missingClientEnvDelimMsg,   // 2
            noBrokerMessageMsg,         // 3
            noRequestTypeMsg,           // 4
            noRequestIdMsg,             // 5
            unknownWorkerMsg,           // 6
            unknownRequestTypeMsg,      // 7
            finishedWithPlus1CapMsg,    // 8
            finishedWith1CapMsg,        // 9
            finishedWithNoPendReqMsg,   // 10
            sendToClientFailsMsg,       // 11
            unknownRequestIdMsg,        // 12
            missingVFS,                 // 13
            nullptr
        };

        senderReceiver.sendReturnValues = {
            0
        };

        // 1 - no client env -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 2 - no client env delimiter -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 3 - no broker message -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 4 - no request type -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        // 5 - no request id -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );

        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::SyncResponsesSent ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        // 6 - unknown worker
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        // 7 -> worker0 should pick up rt0/rid2
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 7, senderReceiver.sentMessages.size() );
#undef checkBrokerState                                     // and one sent for worker request
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 1, 2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 )
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_EQ( (TestZMQSocket*) lpBroker->backendMessagingSocket,
                   senderReceiver.sentMessages[ 6 ].first );
        ASSERT_NO_FATAL_FAILURE( checkForwardedClientRequest(
            __LINE__,
            frameHandler,
            senderReceiver.sentMessages[ 6 ].second,
            { "worker0" },
            env2,
            vfs2,
            rt0,
            rid2,
            data ) );

        // 8 finshed request with >1 capacity afterwards -> worker2 worker2 should 
        //                                                  pick up rt1/rid3 rt2/rid2 requests
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 0, 0 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid2 ) );

        // worker1 should not pick up anything, free capacity should be 1 and should
        // be available for both rt0 and rt1 requests
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt1,
            rid1 ) );

        // worker3 should pick up nothing
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid2, \
            workerInfo3 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid1 ) );

        // TODO - richer testing API - i.e. senderReceiver.nextReturnValue( .. ) ..
        //        instead of this crap
        senderReceiver.sendReturnValues = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,                      // fail one response send
            0
        };

        // should not affect state
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
#undef checkBrokerState
#define checkBrokerState( line ) \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 0, workerInfo1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid2, \
            workerInfo0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid3, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid2, \
            workerInfo2 );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerNotInProgressRequest(
            __LINE__,
            *lpBroker,
            rt3,
            rid2 ) );

        // should not have an affect on anything
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        // 13 - no vfs -> malformed response
        ret = lpBroker->handleBackendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::MalformedWorkerResponses ] );

        STATS_ASSERT_EQ( 9, lpBroker->stats[ BrokerStat::AsyncRequestsCompleted ] );
        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::SyncResponsesSent ] );
        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::SyncResponsesSentFailed ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ResponsesFromUnknownWorkers ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ResponsesForUnknownReqTypes ] );

        ASSERT_EQ( 0, workerInfo0->FreeCapacity() );
        ASSERT_EQ( 1, workerInfo1->FreeCapacity() );
        ASSERT_EQ( 0, workerInfo2->FreeCapacity() );
        ASSERT_EQ( 4, workerInfo3->FreeCapacity() ); // it ends up one higher due to 
                                                    // the unknown request
    }

    TEST_F( TestBroker, TestHandleClientRequest )
    {
        // Main cases
        // Trivial Error cases:
        // 1. Envelope is not null delimited
        // 2. Request Type missing
        // 3. Request Id missing
        // Complex cases:
        // 0. RequestTypeContext exists, request is in progress
        //      a.  ++BrokerStat::ClientRedirectsReceived
        //      b.  Send entire request to worker working on it
        //      c.  ++BrokerStat::WorkerRedirectsSent
        //      d.  worker still working on request
        //      e.  worker capacity unchanged
        // 1. RequestTypeContext exists, request is in progress, failed to send redirect
        //      a.  ++BrokerStat::ClientRedirectsReceived
        //      b.  Send entire request to worker working on it
        //      c.  ++BrokerStat::WorkerRedirectsSentFailed
        //      d.  worker still working on request
        // 2. RequestTypeContext exists, available worker, worker has >1 capacity
        //      a.  ++RequestTypeStat::RequestsReceivedFor
        //      b.  Send to next available worker
        //      c.  request is in progress
        //      d.  worker capacity reduced by 1
        // 3. RequestTypeContext exists, available worker, worker capacity becomes 0
        //      a.  ++RequestTypeStat::RequestsReceivedFor
        //      b.  Send to next available worker
        //      c.  request is in progress
        //      d.  worker removed from all request contexts as available
        // 4. RequestTypeContext exists, available worker, failed to send
        //      a.  ++RequestTypeStat::RequestsReceivedFor
        //      b.  Send to next available worker and fail
        //      c.  request becomes pending
        //      d.  capacity of worker unchanged
        // 5. RequestTypeContext exists, no worker available
        //      a.  ++RequestTypeStat::RequestsReceivedFor
        //      b.  request becomes pending
        // 6. RequestTypeContext does not exist
        //      a.  context created
        //      b.  request becomes pending
        //      c.  ++BrokerStat::RequestTypesAdded
        //      d.  ++RequestTypeStat::RequestsReceivedFor
        //  7.  Request already pending with worker available ... ?
        //      a.  no idea what happens
        //  8.  Request already pending with no workers available ... ?
        //      a.  no idea what happens

        ZMQFrameHandler frameHandler;
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        RequestType rt0 = 10,
                    rt1 = 1,
                    rt2 = 2,
                    rt3 = 3,
                    rtUnknown = 999;
        auto lpBroker( createBroker( socketFactory, senderReceiver ) );
        auto lpWorkerInfo0 = createWorkerInfo( frameHandler, "worker0", 0, {rt0, rt1, rt2, rt3} );
        auto lpWorkerInfo1 = createWorkerInfo( frameHandler, "worker1", 2, {rt0, rt1} );
        auto lpWorkerInfo2 = createWorkerInfo( frameHandler, "worker2", 3, {rt2, rt1} );
        auto lpWorkerInfo3 = createWorkerInfo( frameHandler, "worker3", 3, {rt3} );
        auto workerInfo0 = lpWorkerInfo0.get();
        auto workerInfo1 = lpWorkerInfo1.get();
        auto workerInfo2 = lpWorkerInfo2.get();
        auto workerInfo3 = lpWorkerInfo3.get();

        RequestId rid1 = 1,
                  rid2 = 2,
                  rid3 = 3,
                  ridUnknown = 0;

        vector< const char* > emptyEnv{};
        vector< const char* > env1{ "enva", "envb", "envc" };
        vector< const char* > env2{ "env2a", "env2b", "env2c" };
        vector< const char* > env3{ "env3a" };
        vector< const char* > data1{ "dataa" };
        vector< const char* > data2{ "data2a" };
        vector< const char* > data3{ "data3a", "data3b", "data3c" };

        LHCVersionFlags vfs( 103, 809, MessageFlag::None );
        LHCVersionFlags vfsAllowRedirect( 13, 89, MessageFlag::AllowRedirect );

        int ret = 0;

        // 1. Envelope is not null delimited
        ZMQMessage* msgMissingEnvDelim( nullptr );
        // 2. Request Type missing
        ZMQMessage* msgMissingRT( nullptr );
        // 3. Request Id missing
        ZMQMessage* msgMissingRID( nullptr );
        // 0. RequestTypeContext exists, request is in progress
        ZMQMessage* msgRTExistsRIDInProgRed( nullptr );
        // 1. RequestTypeContext exists, request is in progress, failed to send redirect
        ZMQMessage* msgRTExistsRIDInProgRedFailed( nullptr );
        // 2. RequestTypeContext exists, available worker, worker has >1 capacity
        ZMQMessage* msgRTExistsAvailWorkerGT1Cap( nullptr );
        // 3. RequestTypeContext exists, available worker, worker capacity becomes 0
        ZMQMessage* msgRTExistsAvailWorkerEQ1Cap( nullptr );
        // 4. RequestTypeContext exists, available worker, failed to send
        ZMQMessage* msgRTExistsAvailWorkerSendFail( nullptr );
        // 5. RequestTypeContext exists, no worker available
        ZMQMessage* msgRTExistsNoWorker( nullptr );
        // 6. RequestTypeContext does not exist
        ZMQMessage* msgRTNotExists( nullptr );
        // 7. RequestTypeContext exists and request is pending with avail worker
        ZMQMessage* msgRTExistsWorkerAvailRIDPending( nullptr );
        // 8. RequestTypeContext exists and request is pending with no workers
        ZMQMessage* msgRTExistsNoWorkerRIDPending( nullptr );


        // create an easier function to do this rather than relying on the
        // correct behavior of updateContextsForNewWorker
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo0 ) );
        lpBroker->updateContextsForNewWorker( workerInfo0 );
        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

        // for worker 1
        addRequest( __LINE__,  *lpBroker, env1,
                    MessageFlag::None, 
                    rt0, rid1,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env1,
                    MessageFlag::None, 
                    rt1, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo1 ) );
        lpBroker->updateContextsForNewWorker( workerInfo1 );

        // for worker 2
        addRequest( __LINE__,  *lpBroker, env1,
                    MessageFlag::None, 
                    rt1, rid2,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    MessageFlag::AllowRedirect, 
                    rt2, rid1,
                    data2 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo2 ) );
        lpBroker->updateContextsForNewWorker( workerInfo2 );

        // for worker 3
        addRequest( __LINE__,  *lpBroker, env2,
                    MessageFlag::AllowRedirect, 
                    rt3, rid1,
                    data1 );
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo3 ) );
        lpBroker->updateContextsForNewWorker( workerInfo3 );

        addRequest( __LINE__,  *lpBroker, env2,
                    MessageFlag::Asynchronous, 
                    rt0, ridUnknown,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    MessageFlag::Asynchronous, 
                    rt1, ridUnknown,
                    data2 );
        addRequest( __LINE__,  *lpBroker, env2,
                    MessageFlag::Asynchronous, 
                    rt2, ridUnknown,
                    data3 );

        //  Worker  RT  RID         Free
        //  worker0 -   -           0
        //  worker1 rt0 rid1         
        //  worker1 rt1 rid1        0
        //  worker2 rt1 rid2        
        //  worker2 rt2 rid1        1
        //  worker3 rt3 rid1        2
        //  -       rt0 ridUnknown
        //  -       rt1 ridUnknown
        //  -       rt2 ridUnknown

#undef checkBrokerState
#define checkBrokerState( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 1, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 1, 1, workerInfo2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); } while( false ) 

        // 1. Envelope is not null delimited
        msgMissingEnvDelim =  createClientNewRequest(
            frameHandler, env1, false, &vfs, &rtUnknown, &rid1, data1, true );
        // 2. Request Type missing
        msgMissingRT =  createClientNewRequest(
            frameHandler, env1, true, &vfs, nullptr, &rid1, data1, true );
        // 3. Request Id missing
        msgMissingRID =  createClientNewRequest(
            frameHandler, env1, true, &vfs, &rtUnknown, nullptr, data1, true );
        // 0. RequestTypeContext exists, request is in progress (+vfs does not matter)
        msgRTExistsRIDInProgRed =  createClientNewRequest(
            frameHandler, env1, true, &vfsAllowRedirect, &rt3, &rid1, data1, true );
        // 1. RequestTypeContext exists, request is in progress, failed to send redirect
        msgRTExistsRIDInProgRedFailed =  createClientNewRequest(
            frameHandler, env2, true, &vfsAllowRedirect, &rt2, &rid1, data2, true );
        // 2. RequestTypeContext exists, available worker, worker has >1 capacity
        msgRTExistsAvailWorkerGT1Cap =  createClientNewRequest(
            frameHandler, env1, true, &vfs, &rt3, &rid2, data1, true );
        // 3. RequestTypeContext exists, available worker, worker capacity becomes 0
        msgRTExistsAvailWorkerEQ1Cap =  createClientNewRequest(
            frameHandler, env3, true, &vfs, &rt2, &rid2, data3, true );
        // 4. RequestTypeContext exists, available worker, failed to send
        msgRTExistsAvailWorkerSendFail =  createClientNewRequest(
            frameHandler, env1, true, &vfsAllowRedirect, &rt3, &rid3, data1, true );
        // 5. RequestTypeContext exists, no worker available
        msgRTExistsNoWorker =  createClientNewRequest(
            frameHandler, env1, true, &vfs, &rt0, &rid2, data1, true );
        // 6. RequestTypeContext does not exist
        msgRTNotExists =  createClientNewRequest(
            frameHandler, env1, true, &vfsAllowRedirect, &rtUnknown, &rid1, data1, true );
        // 7. RequestTypeContext exists and request is pending with avail worker
        msgRTExistsWorkerAvailRIDPending =  createClientNewRequest(
            frameHandler, env1, true, &vfs, &rt3, &rid3, data2, true );
        // 8. RequestTypeContext exists and request is pending with no workers
        msgRTExistsNoWorkerRIDPending =  createClientNewRequest(
            frameHandler, env1, true, &vfs, &rtUnknown, &rid1, data2, true );

        RequestType rt4 = 444;
        LHCVersionFlags vfs_NOR_AR( MessageFlag::NotifyOnRedirect | MessageFlag::AllowRedirect );
        LHCVersionFlags vfs_ROS_AR( MessageFlag::ReceiptOnSend | MessageFlag::AllowRedirect );
        LHCVersionFlags vfs_NOD_ROS_ROD(
            MessageFlag::NotifyOnDeath | MessageFlag::ReceiptOnSend | MessageFlag::RetryOnDeath );
        LHCVersionFlags vfs_ROS( MessageFlag::ReceiptOnSend );

        // needs worker avail -> right to in progress
        ZMQMessage* notifyOnRedirectAllowRedirect_4_1( createClientNewRequest( 
            frameHandler, env1, true, &vfs_NOR_AR, &rt4, &rid1, data1, true ) );

        // no worker available
        ZMQMessage* sendReceiptAllowRedirect_4_2( createClientNewRequest( 
            frameHandler, env1, true, &vfs_ROS_AR, &rt4, &rid2, data1, true ) );
        // add worker to pick up request -> send receipt

        // needs worker avail -> right to in progress, send receipt
        ZMQMessage* notifyOnDeathSendReceipt_4_3( createClientNewRequest( 
            frameHandler, env1, true, &vfs_NOD_ROS_ROD, &rt4, &rid3, data1, true ) );
        // kill worker - send death notify

        // in progress -> notify on redirect and send receipt
        ZMQMessage* sendReceipt_4_1( createClientNewRequest( 
            frameHandler, env2, true, &vfs_ROS, &rt4, &rid1, data2, true ) );
        // kill worker -> nothing

        senderReceiver.receiveReturnValues = {
            msgMissingEnvDelim,
            msgMissingRT,
            msgMissingRID,
            msgRTExistsRIDInProgRed,
            msgRTExistsRIDInProgRedFailed,
            msgRTExistsAvailWorkerGT1Cap,
            msgRTExistsAvailWorkerEQ1Cap,
            msgRTExistsAvailWorkerSendFail,
            msgRTExistsNoWorker,
            msgRTNotExists,
            msgRTExistsWorkerAvailRIDPending,
            msgRTExistsNoWorkerRIDPending,
            notifyOnRedirectAllowRedirect_4_1,
            sendReceiptAllowRedirect_4_2,
            notifyOnDeathSendReceipt_4_3,
            sendReceipt_4_1,
            nullptr
        };

        senderReceiver.sendReturnValues = {
            0, 0, 0, 0, 0, // five messages sent to workers to setup test
            0, 1, // first redirect succeeds, second fails
            0, 0, 1, // next two succeed, third fails
            0
        };

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        ASSERT_EQ( 5, senderReceiver.sentMessages.size() );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedFrontendMessages ] );
        STATS_ASSERT_EQ( 0, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::MalformedClientRequests ] );
        ASSERT_EQ( 5, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::MalformedClientRequests ] );
        ASSERT_EQ( 5, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 3, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerRedirectsSent ] );
        ASSERT_EQ( 6, senderReceiver.sentMessages.size() );
        ASSERT_EQ( 6, senderReceiver.sendCount );
        ASSERT_EQ( senderReceiver.sentMessages[ 5 ].first,
                   ( TestZMQSocket* ) lpBroker->backendMessagingSocket );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedClientRequest( __LINE__,
                frameHandler, senderReceiver.sentMessages[ 5 ].second,
                { "worker3" }, env1, vfsAllowRedirect, rt3, rid1, data1 ) );
        // redirect does not change the state
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerRedirectsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::WorkerRedirectsSentFailed ] );
        ASSERT_EQ( 7, senderReceiver.sentMessages.size() );
        ASSERT_EQ( nullptr, ( TestZMQSocket* ) senderReceiver.sentMessages[ 6 ].second );
        // failed redirect does not change state
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );

        ASSERT_EQ( 2, workerInfo3->FreeCapacity() );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::RequestsSent ] );
        ASSERT_EQ( 8, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedClientRequest( __LINE__,
                frameHandler, senderReceiver.sentMessages[ 7 ].second,
                { "worker3" }, env1, vfs, rt3, rid2, data1 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_EQ( 1, workerInfo3->FreeCapacity() );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt3,
                rid2,
                workerInfo3 ) );

        ASSERT_EQ( 1, workerInfo2->FreeCapacity() );

#undef checkBrokerState
#define checkBrokerState( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 0, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); } while( false ) 

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        ASSERT_EQ( 9, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE(
            checkForwardedClientRequest( __LINE__,
                frameHandler, senderReceiver.sentMessages[ 8 ].second,
                { "worker2" }, env3, vfs, rt2, rid2, data3 ) );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_EQ( 0, workerInfo2->FreeCapacity() );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerInProgressRequest(
                __LINE__,
                *lpBroker,
                rt2,
                rid2,
                workerInfo2 ) );

#undef checkBrokerState
#define checkBrokerState( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 1, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); } while( false ) 


        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_EQ( 1, workerInfo3->FreeCapacity() );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerNotInProgressRequest(
                __LINE__,
                *lpBroker,
                rt3,
                rid3 ) );

#undef checkBrokerState
#define checkBrokerState( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt0, \
            2, 2, 0, 2 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt1, \
            3, 3, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt2, \
            2, 2, 0, 1 ); \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt3, \
            2, 2, 1, 1, workerInfo3 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt0, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid1, \
            workerInfo1 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt1, \
            rid2, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt2, \
            rid1, \
            workerInfo2 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt3, \
            rid1, \
            workerInfo3 ); } while( false ) 


        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 8, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerNotInProgressRequest(
                __LINE__,
                *lpBroker,
                rt0,
                rid2 ) );

        ASSERT_EQ( 4, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 4, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 9, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
        ASSERT_EQ( 5, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerNotInProgressRequest(
                __LINE__,
                *lpBroker,
                rtUnknown,
                rid1 ) );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rtUnknown,
                0, 0, 0, 1 ) );

        // request already pending
        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 10, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::ClientDuplicateRequests ] );
        ASSERT_EQ( 5, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rtUnknown,
                0, 0, 0, 1 ) );

        // request already pending
        ret = lpBroker->handleFrontendMessage();
        ASSERT_EQ( 0, ret );
        STATS_ASSERT_EQ( 11, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientRedirectsReceived ] );
        STATS_ASSERT_EQ( 7, lpBroker->stats[ BrokerStat::RequestsSent ] );
        STATS_ASSERT_EQ( 1, lpBroker->stats[ BrokerStat::RequestsSentFailed ] );
        STATS_ASSERT_EQ( 2, lpBroker->stats[ BrokerStat::ClientDuplicateRequests ] );
        ASSERT_EQ( 5, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 5, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );
        ASSERT_EQ( 10, senderReceiver.sentMessages.size() );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState( __LINE__ ) );
        ASSERT_NO_FATAL_FAILURE(
            checkBrokerRequestTypeContext(
                __LINE__,
                *lpBroker,
                rtUnknown,
                0, 0, 0, 1 ) );

        auto lpWorkerInfo4a = createWorkerInfo( frameHandler, "worker4a", 1, {rt4} );
        auto lpWorkerInfo4b = createWorkerInfo( frameHandler, "worker4b", 1, {rt4} );
        auto lpWorkerInfo4c = createWorkerInfo( frameHandler, "worker4c", 1, {rt4} );
        auto lpWorkerInfo4d = createWorkerInfo( frameHandler, "worker4d", 1, {rt4} );
        auto workerInfo4a = lpWorkerInfo4a.get();
        auto workerInfo4b = lpWorkerInfo4b.get();
        auto workerInfo4c = lpWorkerInfo4c.get();
        auto workerInfo4d = lpWorkerInfo4d.get();

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            1, 1, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid1, \
            workerInfo4a ); } while( false ) 

        // needs worker avail -> right to in progress
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4a ) );
        lpBroker->updateContextsForNewWorker( workerInfo4a );

        ASSERT_EQ( 6, lpBroker->requestTypeContextLookup.size() );
        STATS_ASSERT_EQ( 6, lpBroker->stats[ BrokerStat::RequestTypesAdded ] );

        // notifyOnRedirectAllowRedirect_4_1,
        ret = lpBroker->handleFrontendMessage();
        STATS_ASSERT_EQ( 12, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState2( __LINE__ ) );
        ASSERT_EQ( 11, senderReceiver.sentMessages.size() ); // + req to worker

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            2, 2, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid1, \
            workerInfo4a ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid2, \
            workerInfo4b ); } while( false ) 

        // no worker available
        // sendReceiptAllowRedirect_4_2,
        ret = lpBroker->handleFrontendMessage();
        STATS_ASSERT_EQ( 13, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext( 
            __LINE__, 
            *lpBroker, 
            rt4, 
            1, 1, 0, 1 ); );
        // add worker to pick up request -> send receipt
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4b ) );
        lpBroker->updateContextsForNewWorker( workerInfo4b );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState2( __LINE__ ) );
        ASSERT_EQ( 13, senderReceiver.sentMessages.size() ); // + req to worker, receipt to client

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            3, 3, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid3, \
            workerInfo4c ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid1, \
            workerInfo4a ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid2, \
            workerInfo4b ); } while( false ) 


        // needs worker avail -> right to in progress, send receipt
        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4c ) );
        lpBroker->updateContextsForNewWorker( workerInfo4c );
        // notifyOnDeathSendReceipt_4_3,
        ret = lpBroker->handleFrontendMessage();
        STATS_ASSERT_EQ( 14, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState2( __LINE__ ) );
        ASSERT_EQ( 15, senderReceiver.sentMessages.size() ); // + req to worker, receipt to client
        // kill worker - send another request (no receipt) to another worker
        lpBroker->updateContextsForDeadWorker( workerInfo4c );

        ASSERT_NO_FATAL_FAILURE( checkBrokerRequestTypeContext( 
            __LINE__, 
            *lpBroker, 
            rt4, 
            2, 3, 0, 1 ) ); 

        lpBroker->workerInfoLookup.InsertWorkerInfo( move( lpWorkerInfo4d ) );
        lpBroker->updateContextsForNewWorker( workerInfo4d );

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            3, 4, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid3, \
            workerInfo4d ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid1, \
            workerInfo4a ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid2, \
            workerInfo4b ); } while( false ) 

        ASSERT_EQ( 16, senderReceiver.sentMessages.size() ); // + retry request
        ASSERT_NO_FATAL_FAILURE( checkBrokerState2( __LINE__ ) );
        lpBroker->updateContextsForDeadWorker( workerInfo4d );
        checkBrokerRequestTypeContext( 
            __LINE__, 
            *lpBroker, 
            rt4, 
            2, 4, 0, 0 ); 
        ASSERT_EQ( 17, senderReceiver.sentMessages.size() ); // + death notify

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            2, 4, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid1, \
            workerInfo4a ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid2, \
            workerInfo4b ); } while( false ) 

        // sendReceipt_4_1,
        ret = lpBroker->handleFrontendMessage();
        STATS_ASSERT_EQ( 15, lpBroker->stats[ BrokerStat::RequestsReceived ] );
        ASSERT_NO_FATAL_FAILURE( checkBrokerState2( __LINE__ ) );
        ASSERT_EQ( 20, senderReceiver.sentMessages.size() ); // + red worker, receipt, notify red
        // in progress -> notify on redirect and send receipt
        // kill worker -> nothing
        lpBroker->updateContextsForDeadWorker( workerInfo4a );

#undef checkBrokerState2
#define checkBrokerState2( line ) do { \
        checkBrokerRequestTypeContext( \
            line, \
            *lpBroker, \
            rt4, \
            1, 4, 0, 0 ); \
        checkBrokerInProgressRequest( \
            line, \
            *lpBroker, \
            rt4, \
            rid2, \
            workerInfo4b ); } while( false ) 

#ifdef STATS
        lpBroker->dumpStats();
        lpBroker->dumpStats();
#endif

        // TODO - check contents of all messages rather than just counts

    }
}
