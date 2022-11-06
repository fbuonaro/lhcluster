#include <lhcluster/cluster.h>
#include <lhcluster_impl/broker.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

#include <cstring>

namespace LHClusterNS
{
    namespace Impl
    {
#ifdef STATS
        int dumpStatsBrokerReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpBroker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Broker*)lpBroker )->updateCurrentTime();
                return ( (Broker*)lpBroker )->dumpStats();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Fatal,
                        "Broker.dumpStatsBrokerReactor: uncaught exception[ "
                        << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.dumpStatsBrokerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.dumpStatsBrokerReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }
#endif

        int heartbeatWorkersReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpBroker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Broker*)lpBroker )->updateCurrentTime();
                return ( (Broker*)lpBroker )->sendWorkerHeartbeats();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Broker.heartbeatWorkersReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.heartbeatWorkersReactor: uncaught exception[ "
                            << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.heartbeatWorkersReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.heartbeatWorkersReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }

        int frontendMessagingReactor(
            ZMQReactor* reactor,
            SocketRouter* routerSocket,
            void* lpBroker )
        {
            (void)reactor;
            (void)routerSocket;
            try
            {
                return ( (Broker*)lpBroker )->handleFrontendMessage();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Broker.frontendMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.frontendMessagingReactor: uncaught exception[ "
                            << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.frontendMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.frontendMessagingReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        int backendMessagingReactor(
            ZMQReactor* reactor,
            SocketRouter* routerSocket,
            void* lpBroker )
        {
            (void)reactor;
            (void)routerSocket;
            try
            {
                return ( (Broker*)lpBroker )->handleBackendMessage();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Broker.backendMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.backendMessagingReactor: uncaught exception[ "
                            << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.backendMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.backendMessagingReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        int brokerPrimaryControllerReactor(
            ZMQReactor* reactor,
            SocketPair* listenerSocket,
            void* lpBroker )
        {
            (void)reactor;
            (void)listenerSocket;
            (void)lpBroker;
            int ret = -1;
            // ZMQMessage* signal_msg = nullptr;
            // Broker* broker = ( Broker* ) lpBroker;

            // ignore the activity, just exit for now
            // because anythign received over the pair socket means "stop"
            return ret;
        }

        int checkWorkersPulseReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpBroker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Broker*)lpBroker )->updateCurrentTime();
                return ( (Broker*)lpBroker )->checkWorkersPulse();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Broker.checkWorkersPulseReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.checkWorkersPulseReactor: uncaught exception[ "
                            << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.checkWorkersPulseReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker.checkWorkersPulseReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        const char* BrokerConstructionFailed::what() const throw( )
        {
            return "Failed to construct broker";
        }

        Broker::Broker( const Endpoint& _selfFrontendMessagingEndpoint,
            const Endpoint& _selfBackendMessagingEndpoint,
            const Endpoint& _selfControllerEndpoint,
            Delay _selfHeartbeatDelay,
            Delay _minimumWorkerHeartbeatDelay,
            Delay _workerPulseDelay,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver )
            : IBroker()
#ifdef STATS
            , stats()
#endif
            , selfBackendMessagingEndpoint( _selfBackendMessagingEndpoint )
            , selfFrontendMessagingEndpoint( _selfFrontendMessagingEndpoint )
            , selfControllerEndpoint( _selfControllerEndpoint )
            , backendMessagingSocket( _socketFactory->Create( SocketType::Router ) )
            , frontendMessagingSocket( _socketFactory->Create( SocketType::Router ) )
            , listenerSocket( _socketFactory->Create( SocketType::Pair ) )
            , signalerSocket( _socketFactory->Create( SocketType::Pair ) )
            , workerInfoLookup()
            , reactor( zloop_new() )
            , selfHeartbeatDelay( _selfHeartbeatDelay )
            , workerDeathDelay( 2 * _selfHeartbeatDelay.count() )
            , minimumWorkerHeartbeatDelay( _minimumWorkerHeartbeatDelay )
            , workerPulseDelay( _workerPulseDelay )
            , heartbeatWorkerReactorId( -1 )
            , checkPulseWorkerReactorId( -1 )
#ifdef STATS
            , dumpStatsReactorId( -1 )
#endif
            , reactorThread( nullptr )
            , requestTypeContextLookup()
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
        {
            if ( !( backendMessagingSocket && listenerSocket &&
                signalerSocket && reactor ) )
            {
                throw BrokerConstructionFailed();
            }
        }

        Broker::~Broker()
        {
            try
            {
                Stop();

                if ( reactor )
                {
                    zloop_destroy( &reactor );
                }

                if ( backendMessagingSocket )
                {
                    socketFactory->Destroy( &backendMessagingSocket );
                }

                if ( listenerSocket )
                {
                    socketFactory->Destroy( &listenerSocket );
                }

                if ( signalerSocket )
                {
                    socketFactory->Destroy( &signalerSocket );
                }

                if ( frontendMessagingSocket )
                {
                    socketFactory->Destroy( &frontendMessagingSocket );
                }
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Broker.destructor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Broker: exception thrown in destructor" );
                }
                catch ( ... )
                {
                }
            }
        }

        void Broker::Stop()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "Broker.stop" )

                clusterLog( "Broker.stop: begin" );

            if ( reactorThread )
            {
                int ret = senderReceiver->Signal( signalerSocket, 0 );
                checkRet( ret, 0,
                    "Broker.stop: failed to signal stop[" << ret << "]" );

                if ( reactorThread->joinable() )
                {
                    clusterLog( "Broker.stop: joining" );
                    reactorThread->join();
                    clusterLog( "Broker.stop: joined" );
                }
#ifdef DEBUG
                else
                {
                    clusterLog( "Broker.stop: not joining" );
                }
#endif

                reactorThread = nullptr;
            }

            ret = teardownControllerCallbacks();
            checkRet( ret, 0,
                "Broker.stop: failed to td controller cbs[" << ret << "]" );

            ret = deactivateControllerSockets();
            checkRet( ret, 0,
                "Broker.stop: failed to deactivate controller sockets["
                << ret
                << "]" );

            clusterLog( "Broker.stop: end" );
        }

        int Broker::activateControllerSockets()
        {
            int ret = 0;
            int ret1 = 0;

            ret = socketFactory->Bind( listenerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Broker.activateControllerSockets: failed to bind listener["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            ret1 = socketFactory->Connect( signalerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret1, 0,
                "Broker.activateControllerSockets: failed to connect signaler"
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            ret += ret1;

            return ret;
        }

        int Broker::activateMessagingSockets()
        {
            int ret = 0;
            int ret1 = 0;

            //bind the backend router address and start listening for worker messages
            ret = socketFactory->Bind(
                backendMessagingSocket, selfBackendMessagingEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Broker.activateMessagingSockets: failed to bind backend"
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            ret1 = socketFactory->Bind(
                frontendMessagingSocket, selfFrontendMessagingEndpoint.path() ) < 0;
            checkRet( ret1, 0,
                "Broker.activateMessagingSockets: failed to bind frontend"
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            ret += ret1;

            return ret;
        }

        int Broker::setupControllerCallbacks()
        {
            int ret = 0;

            ret = zloop_reader(
                reactor,
                listenerSocket,
                brokerPrimaryControllerReactor,
                this );

            return ret;
        }

        int Broker::teardownControllerCallbacks()
        {
            int ret = 0;

            zloop_reader_end( reactor, listenerSocket );

            return ret;
        }

        int Broker::teardownMessagingCallbacks()
        {
            int ret = 0;

            zloop_reader_end( reactor, frontendMessagingSocket );
            zloop_reader_end( reactor, backendMessagingSocket );
            zloop_timer_end( reactor, heartbeatWorkerReactorId );
            zloop_timer_end( reactor, checkPulseWorkerReactorId );
#ifdef STATS
            zloop_timer_end( reactor, dumpStatsReactorId );
#endif

            return ret;
        }

        int Broker::setupMessagingCallbacks()
        {
            int ret = 0;
            int ret1 = 0;
            std::chrono::milliseconds minimumWorkerHeartbeatDelay_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    minimumWorkerHeartbeatDelay );
            std::chrono::milliseconds workerPulseDelay_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    workerPulseDelay );
#ifdef STATS
            std::chrono::milliseconds dumpStatsDelay_ms =
                std::chrono::milliseconds( 5000 );
#endif


            // Setup reactor callback functions, callback functions
            // must be friends of the broker class
            ret = zloop_reader(
                reactor,
                backendMessagingSocket,
                backendMessagingReactor,
                this );
            checkRet( ret, 0,
                "Broker.setupMessagingCallbacks: "
                "failed to backend reactor" );

            ret1 = zloop_reader(
                reactor,
                frontendMessagingSocket,
                frontendMessagingReactor,
                this );
            checkRet( ret1, 0,
                "Broker.setupMessagingCallbacks: "
                "failed to setup frontend reactor" );
            ret += ret1;

            heartbeatWorkerReactorId =
                zloop_timer(
                    reactor,
                    minimumWorkerHeartbeatDelay_ms.count(),
                    0,
                    heartbeatWorkersReactor,
                    this );

            checkPulseWorkerReactorId =
                zloop_timer(
                    reactor,
                    workerPulseDelay_ms.count(),
                    0,
                    checkWorkersPulseReactor,
                    this );

#ifdef STATS
            dumpStatsReactorId =
                zloop_timer(
                    reactor,
                    dumpStatsDelay_ms.count(),
                    0,
                    dumpStatsBrokerReactor,
                    this );
#endif

            return ret;
        }

        int Broker::deactivateControllerSockets()
        {
            int ret = 0;
            int ret1 = 0;

            //disconnect and unbind pairs
            socketFactory->DisConnect( signalerSocket, selfControllerEndpoint.path() );
            socketFactory->UnBind( listenerSocket, selfControllerEndpoint.path() );

            ret = RefreshSocket( *socketFactory, &signalerSocket, SocketType::Pair );
            checkRet( ret, 0,
                "Broker.deactivateControllerSockets: "
                "failed to refresh signaler" );

            ret1 = RefreshSocket( *socketFactory, &listenerSocket, SocketType::Pair );
            checkRet( ret1, 0,
                "Broker.deactivateControllerSockets: "
                "failed to refresh listener" );

            ret += ret1;

            return ret;
        }

        int Broker::deactivateMessagingSockets()
        {
            int ret = 0;

            //unbind backend router socket
            socketFactory->UnBind(
                backendMessagingSocket, selfBackendMessagingEndpoint.path() );
            socketFactory->UnBind(
                frontendMessagingSocket, selfFrontendMessagingEndpoint.path() );

            return ret;
        }

        void Broker::Start()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "Broker.start" )

                clusterLog( "Broker.start: begin" );

            clusterLog( "Broker.start: starting with parameters\n" <<
                "    selfFrontendMessagingEndpoint[" << selfFrontendMessagingEndpoint << "]\n" <<
                "    selfBackendMessagingEndpoint[" << selfBackendMessagingEndpoint << "]\n" <<
                "    selfControllerEndpoint[" << selfControllerEndpoint << "]\n" <<
                "    selfHeartbeatDelay[" << selfHeartbeatDelay.count() << "]\n" <<
                "    minimumWorkerHeartbeatDelay[" << minimumWorkerHeartbeatDelay.count() << "]\n" <<
                "    workerPulseDelay[" << workerPulseDelay.count() << "]" );

            ret = activateControllerSockets();
            checkRet( ret, 0, "Broker.start: failed to activate controller sockets" );

            ret = setupControllerCallbacks();
            checkRet( ret, 0, "Broker.start: failed to su controller cbs" );

            // std::ref is required because worker is not
            // copyable but thread only allows pass by value
            reactorThread = std::unique_ptr<std::thread>(
                new std::thread( &Broker::StartAsPrimary, std::ref( *this ) ) );

            if ( !( reactorThread ) )
            {
                ret = teardownControllerCallbacks();
                checkRet( ret, 0, "Broker.start: failed to td controller cbs" );

                ret = deactivateControllerSockets();
                checkRet( ret, 0,
                    "Broker.start: failed to deactivate controller sockets" );

                clusterLog( "Broker.start: failed to start" );
            }
            else
            {
                clusterLog( "Broker.start: end" );
            }
        }

        void Broker::StartAsPrimary()
        {
            clusterLogSetActor( selfControllerEndpoint.path() )

                clusterLog( "Broker.startAsPrimary: begin" );

            int ret = 0;

            ret = activateMessagingSockets();
            checkRet( ret, 0,
                "Broker.startAsPrimary: failed to activate messaging sockets" );

            ret = setupMessagingCallbacks();
            checkRet( ret, 0, "Broker.startAsPrimary: failed to su messaging cbs" );

#ifdef STATS
            dumpStatsHeaders();
#endif

            clusterLog( "Broker.startAsPrimary: entering reactor loop" );

            ret = zloop_start( reactor );
            checkRet( ret, 0, "Broker.startAsPrimary: failed to start loop" );

            clusterLog(
                "Broker.startAsPrimary: leaving reactor loop ["
                << ret
                << "|"
                << zmq_strerror( errno )
                << "]" );

            ret = teardownMessagingCallbacks();
            checkRet( ret, 0, "Broker.startAsPrimary: failed to td messaging cbs" );

            ret = deactivateMessagingSockets();
            checkRet( ret, 0,
                "Broker.startAsPrimary: "
                "failed to deactivate messaging sockets" );

            workerInfoLookup.Clear();

            clusterLog( "Broker.startAsPrimary: end" );
        }

        int Broker::sendWorkerHeartbeats()
        {
            clusterLogSetAction( "Broker.sendWorkerHeartbeats" )

                WorkerInfoLookup::WILMapType& workerInfos =
                workerInfoLookup.UnderlyingMap();
            int ret = 0;

            clusterLog( "Broker.sendWorkerHeartbeats: begin" );

            for ( auto it = workerInfos.begin(); it != workerInfos.end(); ++it )
            {
                // this will crash if end iterator is passed
                WorkerInfo* workerInfo =
                    workerInfoLookup.WorkerInfoForIterator( it );
                if ( workerInfo->IsHeartbeatNeededAsOf( currentTime ) )
                {
                    ret = sendWorkerHeartbeat( workerInfo );
                }
#ifdef DEBUG
                else
                {
                    char* hexIdentity = zframe_strhex(
                        const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
                    clusterLog(
                        "Broker.sendWorkerHeartbeat: heartbeat for "
                        << hexIdentity
                        << " not needed at this time" );
                    free( hexIdentity );
                }
#endif
            }

            clusterLog( "Broker.sendWorkerHeartbeats: end" );

            return ret;
        }

        int Broker::checkWorkersPulse()
        {
            int ret = 0;
            WorkerInfoLookup::WILMapType& workerInfos =
                workerInfoLookup.UnderlyingMap();
            auto it = workerInfos.begin();

            clusterLogSetAction( "Broker.checkWorkersPulse" )

                clusterLog( "Broker.checkWorkersPulse: begin" );

            while ( it != workerInfos.end() )
            {
                WorkerInfo* workerInfo =
                    workerInfoLookup.WorkerInfoForIterator( it );
#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
                clusterLog(
                    "Broker.checkWorkersPulse: checking pulse of worker "
                    << hexIdentity );
                free( hexIdentity );
#endif
                if ( workerInfo->IsAliveAsOf( currentTime, workerDeathDelay ) )
                    ++it;
                else
                {
#ifdef DEBUG
                    char* hexIdentity = zframe_strhex(
                        const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
                    clusterLog( "Broker.checkWorkersPulse: worker "
                        << hexIdentity << " has died" );
                    free( hexIdentity );
#endif
                    // remove will invalidate the iterator after
                    // calling remove, so it is necessary to use a postfix iterator
                    // to make a copy so that iteration can continue
                    ret = handleWorkerDeath( workerInfo );
                    IncrementStat( stats, BrokerStat::WorkerDeaths );
                    workerInfoLookup.RemoveWorkerInfo( it++ );
                    //NOTE - reference workerInfo is now invalid
            }
        }

            clusterLog( "Broker.checkWorkersPulse: end" );

            return ret;
    }

        int Broker::handleWorkerReady(
            ZMQFrame** lpIdentity,
            ZMQMessage** lpReadyMsg )
        {

            WorkerInfo* workerInfo = nullptr;
            int ret = 0;

            clusterLogSetAction( "Broker.handleWorkerReady" )
                clusterLog( "Broker.handleWorkerReady: begin" );

            IncrementStat( stats, BrokerStat::WorkerReadies );

            workerInfo = workerInfoLookup.LookupWorkerInfo( *lpIdentity );
            if ( workerInfo )
            {
#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
                clusterLog( "Broker.handleWorkerReady:  worker "
                    << hexIdentity << " sent extra ready" );
                free( hexIdentity );
#endif
                zframe_destroy( lpIdentity );
        }
            else
            {
                ZMQFrameHandler frameHandler;
                std::unique_ptr< WorkerInfo > lpNewWorkerInfo = nullptr;
                ZMQMessage* workerReadyMsg = *lpReadyMsg;
                std::vector< RequestType > supportedRequestTypes;
                size_t numberFrames = 0;
                ZMQFrame* workerReadyFrame = nullptr;
                int freeCapacity = 0;
                Delay heartbeatSendDelay( 0 );
                LHCVersionFlags wvfs;

                numberFrames = zmsg_size( workerReadyMsg );
                if ( numberFrames < 4 )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "insufficient number of frames in ready message" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }

                // parse version and flags
                workerReadyFrame = zmsg_first( workerReadyMsg );
                if ( !( workerReadyFrame && zframe_size( workerReadyFrame ) ) )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "version and flags missing or message invalid" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }
                wvfs = frameHandler.get_frame_value< LHCVersionFlags >( workerReadyFrame );

                //Parsing the freeCapacity
                workerReadyFrame = zmsg_next( workerReadyMsg );
                if ( !( workerReadyFrame && zframe_size( workerReadyFrame ) ) )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "free capacity missing or message invalid" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }
                freeCapacity = frameHandler.get_frame_value< int >( workerReadyFrame );
                if ( freeCapacity < 0 )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "free capacity cannot be negative" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }

                //Parsing the heartbeatSendDelay
                workerReadyFrame = zmsg_next( workerReadyMsg );
                if ( !( workerReadyFrame && zframe_size( workerReadyFrame ) ) )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "heartbeat send delay missing or message invalid" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }

                // TODO - validate send delay - cannot be too large or negative or less than min
                heartbeatSendDelay =
                    Delay( frameHandler.get_frame_value< int >( workerReadyFrame ) );

                //Parsing the first supported_requesttype
                supportedRequestTypes.reserve( numberFrames - 2 );
                //Parsing remaining request types
                while ( ( workerReadyFrame = zmsg_next( workerReadyMsg ) ) &&
                    zframe_size( workerReadyFrame ) )
                {
                    supportedRequestTypes.push_back(
                        frameHandler.get_frame_value< RequestType >( workerReadyFrame ) );
                }

                if ( workerReadyFrame )
                {
                    clusterLog( "Broker.handleWorkerReady: "
                        "invalid request type or message invalid" );
                    zframe_destroy( lpIdentity );
                    zmsg_destroy( lpReadyMsg );
                    IncrementStat( stats, BrokerStat::MalformedWorkerReadies );
                    return 1;
                }

                lpNewWorkerInfo = std::unique_ptr< WorkerInfo >(
                    new WorkerInfo(
                        lpIdentity,
                        currentTime,
                        freeCapacity,
                        move( supportedRequestTypes ),
                        heartbeatSendDelay,
                        wvfs ) );
                workerInfo = lpNewWorkerInfo.get();

                workerInfoLookup.InsertWorkerInfo( std::move( lpNewWorkerInfo ) );
                updateContextsForNewWorker( workerInfo );
            }

            zmsg_destroy( lpReadyMsg );

            clusterLog( "Broker.handleWorkerReady: end[" << ret << "]" );

            return ret;
}

        int Broker::handleWorkerHeartbeat(
            ZMQFrame* identity,
            ZMQMessage** lpHeartbeatMsg )
        {
            WorkerInfo* workerInfo = nullptr;
            ZMQMessage* heartbeatMsg = *lpHeartbeatMsg;
            ZMQFrame* frame = nullptr;
            std::time_t sentAt = 0;
            Delay newHeartbeatSendDelay( 0 );
            int ret = 0;
            ZMQFrameHandler frameHandler;

            clusterLogSetAction( "Broker.handleWorkerHeartbeat" )
                clusterLog( "Broker.handleWorkerHeartbeat: begin" );

            IncrementStat( stats, BrokerStat::WorkerHeartbeatsReceived );

            workerInfo = workerInfoLookup.LookupWorkerInfo( identity );
            if ( !( workerInfo ) )
            {
#ifdef DEBUG
                char* hexIdentity = zframe_strhex( identity );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Warning,
                    "Broker.handleWorkerHeartbeat: unknown worker "
                    << hexIdentity << " sent heartbeat" );
                free( hexIdentity );
#endif
                return 1;
        }


            frame = zmsg_first( heartbeatMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
#ifdef DEBUG
                char* hex_identity = zframe_strhex( identity );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerHeartbeat: unable to update heartbeat"
                    " info for worker "
                    << hex_identity
                    << " because of malformed heartbeat message - "
                    "missing \"sent at\" frame" )
                    free( hex_identity );
#endif
                zmsg_destroy( lpHeartbeatMsg );
                return 2;
            }
            sentAt = frameHandler.get_frame_value< std::time_t >( frame );
            if ( ( sentAt <= 0 ) || ( sentAt > currentTime ) )
            {
#ifdef DEBUG
                char* hex_identity = zframe_strhex( identity );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerHeartbeat: unable to update heartbeat"
                    " info for worker "
                    << hex_identity
                    << " because of malformed heartbeat message - "
                    "invalid \"sent at\" frame[" << sentAt << "]" )
                    free( hex_identity );
#endif
                zmsg_destroy( lpHeartbeatMsg );
                return 2;
            }

            frame = zmsg_next( heartbeatMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
#ifdef DEBUG
                char* hex_identity = zframe_strhex( identity );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerHeartbeat: unable to update heartbeat info for worker "
                    << hex_identity
                    << " because of malformed heartbeat message - "
                    "missing \"heartbeat delay\" frame" )
                    free( hex_identity );
#endif
                zmsg_destroy( lpHeartbeatMsg );
                return 3;
            }

            newHeartbeatSendDelay = Delay( frameHandler.get_frame_value< Delay::rep >( frame ) );
            if ( newHeartbeatSendDelay < minimumWorkerHeartbeatDelay )
            {
#ifdef DEBUG
                char* hex_identity = zframe_strhex( identity );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerHeartbeat: unable to update heartbeat info for worker "
                    << hex_identity
                    << " because of malformed heartbeat message - "
                    "invalid \"heartbeat delay\" frame[" << newHeartbeatSendDelay.count() << "]" )
                    free( hex_identity );
#endif
                zmsg_destroy( lpHeartbeatMsg );
                return 3;
            }

            workerInfo->UpdateWithHeartbeat( currentTime, sentAt, newHeartbeatSendDelay );

            zmsg_destroy( lpHeartbeatMsg );

            clusterLog( "Broker.handleWorkerHeartbeat: end" );

            return ret;
        }

        int Broker::handleFrontendMessage( ZMQMessage** lpFrontendMessage )
        {
            int ret = 0;
            ClusterMessage messageType;
            ZMQFrame* frame = nullptr;
            ZMQFrameHandler frameHandler;
            ZMQMessage* frontendMessage = *lpFrontendMessage;

            clusterLogSetAction( "Broker.handleFrontendMessage" )
                clusterLog( "Broker.handleFrontendMessage: begin" )

                //1) skip the envelope
                frame = zmsg_first( frontendMessage );
            while ( frame && zframe_size( frame ) )
            {
                frame = zmsg_next( frontendMessage );
            }
            //frame should now be the null delimiter

            if ( !( frame && ( zframe_size( frame ) == 0 ) ) )
            {
                clusterLog(
                    "Broker.handleFrontendMessage: "
                    "invalid format or missing envelope delimiter" );
                zmsg_destroy( lpFrontendMessage );
                IncrementStat( stats, BrokerStat::MalformedFrontendMessages );
                return 0;
            }

            //2) move onto the ClusterMessage type and parse it. remove
            //   frame because add broker adds its own message type
            frame = zmsg_next( frontendMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleFrontendMessage: missing client message type" );
                zmsg_destroy( lpFrontendMessage );
                IncrementStat( stats, BrokerStat::MalformedFrontendMessages );
                return 0;
            }

            zmsg_remove( frontendMessage, frame );
            messageType = frameHandler.get_frame_value< ClusterMessage >( frame );
            zframe_destroy( &frame );

            switch ( messageType )
            {
            case ClusterMessage::ClientNewRequest:
            {
                handleClientRequest( lpFrontendMessage );
                break;
            }
            case ClusterMessage::ClientHeartbeat:
            {
                ret = handleClientHeartbeat( lpFrontendMessage );
                break;
            }
            default:
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleFrontendMessage: received unsupported ClusterMessage" );
                zmsg_destroy( lpFrontendMessage );
                break;
            }
            }

            clusterLog( "Broker.handleFrontendMessage: end" )
                return ret;
        }

        int Broker::handleFrontendMessage()
        {
            int ret = 0;
            ZMQMessage* frontendMessage = nullptr;

            clusterLogSetAction( "Broker.handleFrontendMessage" );
            clusterLog( "Broker.handleFrontendMessage: begin" );

            updateCurrentTime();

            frontendMessage = senderReceiver->Receive( frontendMessagingSocket );
            if ( !( frontendMessage ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleFrontendMessage: unable to retrieve message" );
                IncrementStat( stats, BrokerStat::FailedClientReceives );
                return 0;
            }

            ret = handleFrontendMessage( &frontendMessage );
            ret = ret < 0 ? ret : 0;

            clusterLog( "Broker.handleFrontendMessage: end" );
            return ret;
        }

        int Broker::handleBackendMessage( ZMQMessage** lpBackendMessage )
        {
            int ret = 0;
            ClusterMessage messageType;
            ZMQFrame* identityFrame = nullptr;
            ZMQFrame* frame = nullptr;
            ZMQFrameHandler frameHandler;
            ZMQMessage* backendMessage = *lpBackendMessage;

            /*******************************************************************
             * A proper backend message will be composed of four primary parts:
             *  1)  The envelope/identity
             *  2)  The null frame
             *  3)  The ClusterMessage type
             *  4)  The rest of the message ...
             * All proper message are prepended with a null frame to indicate the
             * end of envelope information
             * NOTE - AT THIS TIME, ENVELOPES ARE NOT SUPPORTED AND IT IS ASSUMED THAT
             *        THERE IS ONLY ONE IDENTITY FOLLOWED BY A NULL FRAME
             *
             * The steps taken to parse the message are as follows:
             *  1)  Pop the identity
             *  2)  Pop the null frame (error, log, and
                    throwaway if not null - see prior note )
             *  3)  Pop and parse the worker message type
             *  4)  Switch the message type and
                    handle each case (or no case if invalid) correctly
             */

            clusterLogSetAction( "Broker.handleBackendMessage" );
            clusterLog( "Broker.handleBackendMessage: begin" );

            // 1) Get the identity
            identityFrame = zmsg_pop( backendMessage );
            if ( !( identityFrame && zframe_size( identityFrame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleBackendMessage: "
                    "message is invalid or does not have an identity" );
                if ( identityFrame )
                    zframe_destroy( &identityFrame );
                zmsg_destroy( lpBackendMessage );
                IncrementStat( stats, BrokerStat::MalformedBackendMessages );
                return 0;
            }

            // 2) pop the null delimiter
            frame = zmsg_pop( backendMessage );
            if ( !( frame && ( zframe_size( frame ) == 0 ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleBackendMessage: "
                    "message is invalid or does not have null delimiter" );
                if ( frame )
                    zframe_destroy( &frame );
                zframe_destroy( &identityFrame );
                zmsg_destroy( lpBackendMessage );
                IncrementStat( stats, BrokerStat::MalformedBackendMessages );
                return 0;
            }
            zframe_destroy( &frame );

            // 3) pop and parse the message type
            frame = zmsg_pop( backendMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleBackendMessage: "
                    "message is invalid or does not have a message type" );
                if ( frame )
                    zframe_destroy( &frame );
                zframe_destroy( &identityFrame );
                zmsg_destroy( lpBackendMessage );
                IncrementStat( stats, BrokerStat::MalformedBackendMessages );
                return 0;
            }
            messageType = frameHandler.get_frame_value< ClusterMessage >( frame );
            zframe_destroy( &frame );

            switch ( messageType )
            {
            case ClusterMessage::WorkerReceipt:
            {
                ret = handleWorkerReceipt( identityFrame, lpBackendMessage );
                zframe_destroy( &identityFrame );
                break;
            }
            case ClusterMessage::WorkerHeartbeat:
            {
                /* call method which would:
                 *  error if not yet a registered worker (eventually,
                    send an IP request)
                 *  update worker info last_Active, heartbeat_delay,
                    WILL NEED TO PARSE SAME AS WORKER INFO CTOR
                 */
                ret = handleWorkerHeartbeat( identityFrame, lpBackendMessage );
                zframe_destroy( &identityFrame );
                break;
            }
            case ClusterMessage::WorkerReady:
            {
                /* call method whihc would
                 *  "TRY" to create a new worker info from the rest of the message
                 *  add worker info to lookup
                 */
                ret = handleWorkerReady( &identityFrame, lpBackendMessage );
                break;
            }
            case ClusterMessage::WorkerFinished:
            {
                ret = handleWorkerFinished( &identityFrame, lpBackendMessage );
                break;
            }
            default:
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleBackendMessage: "
                    "unsupported message type received" );
                zframe_destroy( &identityFrame );
                zmsg_destroy( lpBackendMessage );
                IncrementStat( stats, BrokerStat::UnknownWorkerMessages );
                break;
            }
            }

            clusterLog( "Broker.handleBackendMessage: end" );

            return ret;
        }

        int Broker::handleBackendMessage()
        {
            int ret = 0;
            ZMQMessage* backendMessage = nullptr;

            clusterLogSetAction( "Broker.handleBackendMessage" );
            clusterLog( "Broker.handleBackendMessage: begin" );

            updateCurrentTime();

            backendMessage = senderReceiver->Receive( backendMessagingSocket );
            if ( !( backendMessage ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleBackendMessage: unable to retrieve message" );
                IncrementStat( stats, BrokerStat::FailedWorkerReceives );
                return 0;
            }

            ret = handleBackendMessage( &backendMessage );
            ret = ret < 0 ? ret : 0;

            clusterLog( "Broker.handleBackendMessage: end[" << ret << "]" );

            return ret;
        }

        void Broker::updateCurrentTime()
        {
            currentTime = std::time( NULL );
        }

        int Broker::sendWorkerHeartbeat( WorkerInfo* workerInfo )
        {
            int ret = 0;
            ZMQMessage* heartbeatMsg = zmsg_new();
            ZMQFrame* identityFrame =
                zframe_dup( const_cast<ZMQFrame*>(
                    workerInfo->GetIdentity() ) );

            clusterLogSetAction( "Broker.sendWorkerHeartbeat" )
#ifdef DEBUG
                char* hexIdentity =
                zframe_strhex( const_cast<ZMQFrame*>(
                    workerInfo->GetIdentity() ) );
            clusterLog( "Broker.sendWorkerHeartbeat: begin for "
                << hexIdentity );
#endif
            zmsg_prepend( heartbeatMsg, &identityFrame );

            //assume that the send_heartbeat takes care of the message
            ret = SendHeartbeat(
                backendMessagingSocket,
                currentTime,
                selfHeartbeatDelay,
                static_cast<std::uint32_t>( ClusterMessage::BrokerHeartbeat ),
                &heartbeatMsg,
                *senderReceiver );
            if ( ret != 0 )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.sendWorkerHeartbeat: failed to send heartbeat "
                    "to " << hexIdentity );
#endif
                zmsg_destroy( &heartbeatMsg );
                ret = 0;
                IncrementStat( stats, BrokerStat::WorkerHeartbeatsSentFailed );
        }
            else
            {
                workerInfo->UpdateBrokerLastActive( currentTime );
                IncrementStat( stats, BrokerStat::WorkerHeartbeatsSent );
            }

#ifdef DEBUG
            clusterLog( "Broker.sendWorkerHeartbeat: end "
                << hexIdentity
                << "[" << ret << "]" );
            free( hexIdentity );
#endif
            return ret;
        }


        int Broker::updateContextsForNewWorker( WorkerInfo* workerInfo )
        {
            int ret = 0;
            bool thereAreMoreRequests = false;

            clusterLogSetAction( "Broker.updateContextsForNewWorker" )
#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
            clusterLog( "Broker.updateContextsForNewWorker: begin["
                << hexIdentity
                << "|" << workerInfo->FreeCapacity()
                << "]" );
#endif
            // 1a) for each supported request type, add the worker to the request type
            //     context by
            //          i)  inc # workers
            //          ii) add it to context as avail worker
            //          iii)pass it first pending request
            //          iv) if passed request remove from all prev contexts
            //              and only do (i)
            for ( auto it = workerInfo->GetSupportedRequestTypes().begin();
                it != workerInfo->GetSupportedRequestTypes().end();
                ++it )
            {
                BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                    requestTypeContextLookup.find( *it );
                clusterLogSetTag( *it );

                if ( requestTypeAndContext != requestTypeContextLookup.end() )
                {
                    BrokerRequestTypeContext& requestTypeContext =
                        requestTypeAndContext->second;

                    // i) inc # workers
                    requestTypeContext.IncrementNumWorkers();
                    IncrementStat( requestTypeContext.stats,
                        RequestTypeStat::WorkersRegistered );

                    if ( workerInfo->FreeCapacity() > 0 )
                    {
                        RequestState* nextPendingRequest =
                            requestTypeContext.GetNextPendingRequest();

                        if ( nextPendingRequest )
                        {
                            //will add inprog request to workerInfo and context
                            ret = forwardWorkerParsedClientRequest(
                                workerInfo,
                                requestTypeContext,
                                nextPendingRequest );
                            if ( ret == 0 )
                            {
                                clusterLog(
                                    "Broker.updateContextsForNewWorker: "
                                    "forwarded pending request type["
                                    << *it << "]" );

                                if ( workerInfo->FreeCapacity() <= 0 )
                                {
                                    // iv) remove it from any pending queues
                                    //     it may have been on
                                    //     as an optimization we stop at the
                                    //     current request type
                                    for ( auto it2 =
                                        workerInfo->GetSupportedRequestTypes().begin();
                                        it2 != it;
                                        ++it2 )
                                    {
                                        BrokerRequestTypeContextLookup::iterator
                                            requestTypeAndContext2 =
                                            requestTypeContextLookup.find( *it2 );

                                        BrokerRequestTypeContext&
                                            requestTypeContext2 =
                                            requestTypeAndContext2->second;

                                        requestTypeContext2.RemoveAvailableWorker(
                                            workerInfo );
                                    }
                                }
                                else
                                {
                                    // the worker still has remaining capacity
                                    // so add it to the request type as available
                                    requestTypeContext.AddAvailableWorker(
                                        workerInfo );
                                    thereAreMoreRequests =
                                        thereAreMoreRequests ||
                                        ( requestTypeContext.GetNextPendingRequest() !=
                                            nullptr );
                                }
                            }
                            else
                            {
#ifdef DEBUG
                                clusterLog(
                                    "Broker.updateContextsForNewWorker: failed "
                                    "sending "
                                    << hexIdentity
                                    << " pending requests for request type "
                                    << *it );
#endif
                                ret = 0;
                                thereAreMoreRequests = true;    // there is at least this one, try again
                                // in the second sweep
                                requestTypeContext.AddAvailableWorker( workerInfo );
                        }
                    }
                        else
                        {
                            requestTypeContext.AddAvailableWorker( workerInfo );
                        }
                }
            }
                else
                {
                    std::pair< BrokerRequestTypeContextLookup::iterator, bool >
                        insertedValue( requestTypeContextLookup.insert(
                            std::move(
                                BrokerRequestTypeContextLookup::value_type(
                                    *it,
                                    BrokerRequestTypeContext( *it ) ) ) ) );

                    insertedValue.first->second.IncrementNumWorkers();

                    if ( workerInfo->FreeCapacity() > 0 )
                        insertedValue.first->second.AddAvailableWorker(
                            workerInfo );
                    IncrementStat( stats, BrokerStat::RequestTypesAdded );
                    IncrementStat( insertedValue.first->second.stats,
                        RequestTypeStat::WorkersRegistered );

                }
        }

#ifdef DEBUG
            clusterLog(
                "Broker.updateContextsForNewWorker: sweeping in "
                "other pending requests to "
                << hexIdentity );
#endif

            // TODO - this could be a separate function
            // by this point it has been added to every context
            // if the worker has more capacity and thereAreMoreRequests
            // to be processed in supported types then forward the remaining
            // pending requests in a round robin fashion
            while ( ( workerInfo->FreeCapacity() > 0 ) && thereAreMoreRequests )
            {
                thereAreMoreRequests = false;

                // it can be but it can still accept work
                // so cycle through the request types until
                // the capacity is reached or
                for ( auto it = workerInfo->GetSupportedRequestTypes().begin();
                    it != workerInfo->GetSupportedRequestTypes().end();
                    ++it )
                {
                    BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                        requestTypeContextLookup.find( *it );
                    clusterLogSetTag( *it );

                    clusterLog(
                        "Broker.updateContextsForNewWorker: "
                        "sweeping["
                        << *it << "]" );

                    // by this point every req type should be in the context
                    // if( requestTypeAndContext != requestTypeContextLookup.end() )
                    // {
                    BrokerRequestTypeContext& requestTypeContext =
                        requestTypeAndContext->second;

                    RequestState* nextPendingRequest =
                        requestTypeContext.GetNextPendingRequest();

                    if ( nextPendingRequest )
                    {
                        clusterLog(
                            "Broker.updateContextsForNewWorker: "
                            "there are more pending requests["
                            << *it << "]" );
                        //will add inprog request to workerInfo and context
                        ret = forwardWorkerParsedClientRequest(
                            workerInfo,
                            requestTypeContext,
                            nextPendingRequest );
                        if ( ret == 0 )
                        {
                            clusterLog(
                                "Broker.updateContextsForNewWorker: "
                                "forwarded pending request type["
                                << *it << "]" );

                            if ( workerInfo->FreeCapacity() <= 0 )
                            {
                                // iv) remove it from any pending queues
                                //     it may have been on
                                //     as an optimization we stop at the
                                //     current request type
                                for ( auto it2 =
                                    workerInfo->GetSupportedRequestTypes().begin();
                                    it2 <= it;
                                    ++it2 )
                                {
                                    BrokerRequestTypeContextLookup::iterator
                                        requestTypeAndContext2 =
                                        requestTypeContextLookup.find( *it2 );

                                    if ( requestTypeAndContext2 !=
                                        requestTypeContextLookup.end() )
                                    {
                                        BrokerRequestTypeContext&
                                            requestTypeContext2 =
                                            requestTypeAndContext2->second;

                                        requestTypeContext2.RemoveAvailableWorker(
                                            workerInfo );
                                    }
                                }

                                break;
                            }
                            else
                            {
                                thereAreMoreRequests =
                                    thereAreMoreRequests ||
                                    ( requestTypeContext.GetNextPendingRequest() !=
                                        nullptr );
                            }
                        }
                        else
                        {
#ifdef DEBUG
                            clusterLog(
                                "Broker.updateContextsForNewWorker: failed "
                                "sending "
                                << hexIdentity
                                << " pending requests for request type "
                                << *it );
#endif
                            ret = 0;
                    }
                }
                    // }
            }
            }

#ifdef DEBUG
            free( hexIdentity );
#endif
            clusterLog( "Broker.updateContextsForNewWorker: "
                "end[" << ret << "|" << workerInfo->FreeCapacity() << "]" );

            return ret;
        }

        int Broker::handleWorkerDeath( WorkerInfo* workerInfo )
        {
            int ret = 0;

            clusterLogSetAction( "Broker.handleWorkerDeath" );
            clusterLog( "Broker.handleWorkerDeath: begin" );

            ret = updateContextsForDeadWorker( workerInfo );

            clusterLog( "Broker.handleWorkerDeath: end" );

            return ret;
        }

        int Broker::updateContextsForDeadWorker( WorkerInfo* workerInfo )
        {
            int ret = 0;

            clusterLogSetAction( "Broker.updateContextsForDeadWorker" )
#ifdef DEBUG
                char* hexIdentity = zframe_strhex( const_cast<ZMQFrame*>(
                    workerInfo->GetIdentity() ) );
            clusterLog( "Broker.updateContextsForDeadWorker: begin " << hexIdentity );
#endif
            //1) look at each request type context for the supported requesttypes
            for ( auto it = workerInfo->GetSupportedRequestTypes().begin();
                it != workerInfo->GetSupportedRequestTypes().end();
                ++it )
            {
                BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                    requestTypeContextLookup.find( *it );
                clusterLogSetTag( *it );

                if ( requestTypeAndContext != requestTypeContextLookup.end() )
                {
                    BrokerRequestTypeContext& requestTypeContext =
                        requestTypeAndContext->second;
                    RequestType type = requestTypeAndContext->first;
                    auto inProgressRequests = workerInfo->GetInProgressRequests().find( type );

#ifdef DEBUG
                    if ( requestTypeContext.NumWorkersTotal() <= 0 )
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Warning,
                            "Broker.updateContextsForDeadWorker: "
                            << hexIdentity
                            << " died when there are no workers" );
                }
                    else
#endif
                        requestTypeContext.DecrementNumWorkers();

                    IncrementStat( requestTypeContext.stats,
                        RequestTypeStat::WorkersDied );
                    // 1b) remove the worker from the context
                    requestTypeContext.RemoveAvailableWorker( workerInfo );

                    if ( inProgressRequests == workerInfo->GetInProgressRequests().end() )
                    {
                        continue;
                    }

                    // 1c) remove each worker request for the type from the request
                    //     context's in progress requests
                    for ( auto it = inProgressRequests->second.begin();
                        it != inProgressRequests->second.end();
                        ++it )
                    {
                        RequestState* requestState( it->second );
                        RequestId id( it->first );
                        const LHCVersionFlags& vfs( requestState->GetVersionAndFlags() );

#define MAX_TRY_COUNT 2
                        if ( vfs.hasAny( MessageFlag::RetryOnDeath | MessageFlag::NotifyOnDeath ) )
                        {
                            bool retryOnDeathSet = vfs.hasAny( MessageFlag::RetryOnDeath );

                            if ( retryOnDeathSet && ( requestState->GetTryCount() < MAX_TRY_COUNT ) )
                            {
                                // Will attempt again, send to another worker if available else put
                                // into pending
                                // TODO - some of this into a subroutine
                                WorkerInfo* otherWorkerInfo =
                                    requestTypeContext.GetNextAvailableWorker();

                                if ( otherWorkerInfo )
                                {
                                    ret = forwardWorkerParsedClientRequest(
                                        otherWorkerInfo,
                                        id,
                                        vfs,
                                        requestState->GetMessage(),
                                        requestTypeContext );

                                    if ( ret != 0 )
                                    {
#ifdef DEBUG
                                        char* hexIdentity = zframe_strhex(
                                            const_cast<ZMQFrame*>(
                                                otherWorkerInfo->GetIdentity() ) );
                                        clusterLogWithSeverity(
                                            LHClusterNS::SeverityLevel::Error,
                                            "Broker.updateContextsForDeadWorker: "
                                            "failed sending to "
                                            << hexIdentity << " - adding as pending"
                                            << "[" << type << "|" << id << "]" );
                                        free( hexIdentity );
#else
                                        ret = 0;
#endif
                                        requestTypeContext.BackToPending( requestState );
                                }
                                    else
                                    {
                                        if ( otherWorkerInfo->FreeCapacity() <= 0 )
                                        {
                                            for ( auto it2 =
                                                otherWorkerInfo->GetSupportedRequestTypes().begin();
                                                it2 !=
                                                otherWorkerInfo->GetSupportedRequestTypes().end();
                                                ++it2 )
                                            {
                                                BrokerRequestTypeContextLookup::iterator
                                                    requestTypeAndContext2 =
                                                    requestTypeContextLookup.find( *it2 );

                                                if ( requestTypeAndContext2 !=
                                                    requestTypeContextLookup.end() )
                                                {
                                                    BrokerRequestTypeContext& requestTypeContext2 =
                                                        requestTypeAndContext2->second;

                                                    requestTypeContext2.RemoveAvailableWorker(
                                                        otherWorkerInfo );
                                                }
                                            }

                                        }
                                    }
                            }
                                else
                                {
                                    clusterLog(
                                        "Broker.updateContextsForDeadWorker: "
                                        "adding pending request2 due to retry["
                                        << type << "|" << id << "]" );
                                    requestTypeContext.BackToPending( requestState );
                                }
                        }
                            else if ( vfs.hasAny( MessageFlag::NotifyOnDeath ) )
                            {
                                if ( retryOnDeathSet )
                                {
                                    ret = rejectRequest( requestState->GetMessage(),
                                        RequestStatus::WorkerDeath );
                                }
                                else
                                {
                                    ret = rejectRequest( vfs, type, id, requestState->GetMessage(),
                                        RequestStatus::WorkerDeath );
                                }

                                checkRet( ret, 0, "Broker.updateContextsForDeadWorker: "
                                    "failed to send death notification" );

                                clusterLog( "Broker.updateContextsForDeadWorker: failed request["
                                    << type
                                    << "|" << id << "]" );
                                IncrementStat( stats, BrokerStat::RequestsFailedDueToWorkerDeath );
                                IncrementStat( requestTypeContext.stats,
                                    RequestTypeStat::RequestsFailed );
                                requestTypeContext.RemoveInProgressRequest( id );
                            }
                    }
                        else
                        {
                            clusterLog( "Broker.updateContextsForDeadWorker: failed request["
                                << type
                                << "|" << id << "]" );
                            IncrementStat( stats, BrokerStat::RequestsFailedDueToWorkerDeath );
                            IncrementStat( requestTypeContext.stats,
                                RequestTypeStat::RequestsFailed );
                            requestTypeContext.RemoveInProgressRequest( id );
                        }
            }
        }
                else
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "Broker.updateContextsForDeadWorker: "
                        "request type context "
                        << *it
                        << " does not exist" );
                }
            }

#ifdef DEBUG
            clusterLog(
                "Broker.updateContextsForDeadWorker: end "
                << hexIdentity << "[" << ret << "]" );
            free( hexIdentity );
#endif

            return ret;
        }

        int Broker::handleWorkerFinished( ZMQFrame** lpIdentity, ZMQMessage** lpFinishedMsg )
        {
            WorkerInfo* workerInfo = nullptr;
            int ret = 0;
            RequestType requestType = 0;
            RequestId requestId = 0;
            ZMQFrameHandler frameHandler;
            ZMQMessage* finishedMsg = *lpFinishedMsg;
            ZMQFrame* frame = nullptr;
            ZMQFrame* identity = *lpIdentity;
            LHCVersionFlags vfs;

            clusterLogSetAction( "Broker.handleWorkerFinished" )
#ifdef DEBUG
                char* hexIdentity = zframe_strhex( identity );
            clusterLog(
                "Broker.handleWorkerFinished: begin " << hexIdentity );
#endif

            //Skip the client envelope
            frame = zmsg_first( finishedMsg );
            if ( !frame || ( zframe_size( frame ) == 0 ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing envelope" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 1;
        }
            while ( frame && zframe_size( frame ) > 0 )
            {
                frame = zmsg_next( finishedMsg );
            }
            if ( !frame )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing envelope delimiter" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 1;
            }

            //skip the broker message type, only present in response
            frame = zmsg_next( finishedMsg );
            if ( !frame || ( zframe_size( frame ) == 0 ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing broker message type" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 2;
            }

            // 0) version and flags
            frame = zmsg_next( finishedMsg );
            if ( !frame || ( zframe_size( frame ) == 0 ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing version and flags" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 1;
            }
            vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
            // TODO - vfs - clusterLogSetFlags( vfs );

            if ( vfs.hasAny( MessageFlag::Asynchronous ) )
                IncrementStat( stats, BrokerStat::AsyncRequestsCompleted );
            else
                IncrementStat( stats, BrokerStat::SyncRequestsCompleted );

            // 1) request type
            frame = zmsg_next( finishedMsg );
            if ( !frame || ( zframe_size( frame ) == 0 ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing request type" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 3;
            }
            requestType = frameHandler.get_frame_value< RequestType >( frame );
            clusterLogSetTag( requestType );

            // 2) request id
            frame = zmsg_next( finishedMsg );
            if ( !frame || ( zframe_size( frame ) == 0 ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerFinished: malformed response from "
                    << hexIdentity << " - missing request id" )
                    free( hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::MalformedWorkerResponses );
                zmsg_destroy( lpFinishedMsg );
                zframe_destroy( lpIdentity );
                return 4;
            }
            requestId = frameHandler.get_frame_value< RequestId >( frame );

            if ( !( vfs.hasAny( MessageFlag::Asynchronous ) ) )
            {
                clusterLog( "Broker.handleWorkerFinished: sending sync response" )

                    // normally, we would want to add the
                    // ClusterMessage into the middle of the message here
                    // but, because it is very difficult to add to the middle of
                    // the messages I decided to build
                    // the broker message into the message when the
                    // Worker is initially generating the response
                    ret = senderReceiver->Send( lpFinishedMsg, frontendMessagingSocket );
                if ( ret != 0 )
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Error,
                        "Broker.handleWorkerFinished: failed to send" );
                    zmsg_destroy( lpFinishedMsg );
                    IncrementStat( stats, BrokerStat::SyncResponsesSentFailed );
                }
                else
                {
                    IncrementStat( stats, BrokerStat::SyncResponsesSent );
                }
            }
            else
                zmsg_destroy( lpFinishedMsg );

            workerInfo = workerInfoLookup.LookupWorkerInfo( identity );
            if ( !( workerInfo ) )
            {
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Warning,
                    "Broker.handleWorkerFinished: unknown worker "
                    << hexIdentity );
#endif
                IncrementStat( stats, BrokerStat::ResponsesFromUnknownWorkers );
            }
            else
            {
                workerInfo->UpdateWorkerLastActive( currentTime );
                workerInfo->RemoveInProgressRequest( requestType, requestId );
                updateContextsForFinishedWorker( workerInfo, requestType, requestId );
            }

            zframe_destroy( lpIdentity );

#ifdef DEBUG
            clusterLog(
                "Broker.handleWorkerFinished: end "
                << hexIdentity << "[" << ret << "]" );
            free( hexIdentity );
#endif

            return ret;
        }

        int Broker::updateContextsForFinishedWorker(
            WorkerInfo* workerInfo, RequestType type, RequestId id )
        {
            int ret = 0;
            bool removedInProgressRequest = false;
            bool thereAreMoreRequests = false;

            clusterLogSetAction( "Broker.updateContextsForFinishedWorker" )
#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
            clusterLog( "Broker.updateContextsForFinishedWorker: begin["
                << hexIdentity
                << "|" << workerInfo->FreeCapacity()
                << "]" );
#endif


            // similar to the case when updating for a new worker
            // except we do not do anything if capacity >= 2
            // because the assumption is that if there was anything pending
            // for the worker then it would have been passed to it already
            // thereby reducing its capacity to 0

            // then there is room for one or more requests
            // and add it back to pending queues if there are no
            // new requests
            if ( workerInfo->FreeCapacity() > 0 )
            {
                for ( auto it = workerInfo->GetSupportedRequestTypes().begin();
                    it != workerInfo->GetSupportedRequestTypes().end();
                    ++it )
                {
                    BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                        requestTypeContextLookup.find( *it );

                    if ( requestTypeAndContext != requestTypeContextLookup.end() )
                    {
                        BrokerRequestTypeContext& requestTypeContext =
                            requestTypeAndContext->second;

                        RequestState* nextPendingRequest =
                            requestTypeContext.GetNextPendingRequest();

                        if ( !removedInProgressRequest && ( *it == type ) )
                        {
                            requestTypeContext.RemoveInProgressRequest( id );
                            removedInProgressRequest = true;
                            IncrementStat(
                                requestTypeContext.stats,
                                RequestTypeStat::RequestsCompleted );
                        }

                        if ( nextPendingRequest )
                        {
                            //will add inprog request to workerInfo and context
                            ret = forwardWorkerParsedClientRequest(
                                workerInfo,
                                requestTypeContext,
                                nextPendingRequest );
                            if ( ret == 0 )
                            {
                                clusterLog(
                                    "Broker.updateContextsForFinishedWorker: "
                                    "forwarded pending request type["
                                    << *it << "]" );

                                if ( workerInfo->FreeCapacity() <= 0 )
                                {
                                    // iv) remove it from any pending queues
                                    //     it may have been on
                                    //     as an optimization we stop at the
                                    //     current request type
                                    for ( auto it2 =
                                        workerInfo->GetSupportedRequestTypes().begin();
                                        it2 <= it;
                                        ++it2 )
                                    {
                                        BrokerRequestTypeContextLookup::iterator
                                            requestTypeAndContext2 =
                                            requestTypeContextLookup.find( *it2 );

                                        BrokerRequestTypeContext&
                                            requestTypeContext2 =
                                            requestTypeAndContext2->second;

                                        requestTypeContext2.RemoveAvailableWorker(
                                            workerInfo );
                                    }

                                    break;
                                }
                                else
                                {
                                    // the worker still has remaining capacity
                                    // so add it to the request type as available
                                    requestTypeContext.AddAvailableWorker(
                                        workerInfo );
                                    thereAreMoreRequests =
                                        thereAreMoreRequests ||
                                        ( requestTypeContext.GetNextPendingRequest() !=
                                            nullptr );
                                }
                            }
                            else
                            {
#ifdef DEBUG
                                clusterLog(
                                    "Broker.updateContextsForFinishedWorker: failed "
                                    "sending "
                                    << hexIdentity
                                    << " pending requests for request type "
                                    << *it );
#endif
                                ret = 0;
                                thereAreMoreRequests = true;// there is at least this one, try again
                                // in the second sweep
                                requestTypeContext.AddAvailableWorker( workerInfo );
                        }
                    }
                        else
                        {
                            requestTypeContext.AddAvailableWorker( workerInfo );
                        }
                }
                    else
                    {
#ifdef DEBUG
                        clusterLog( "Broker.updateContextsForFinishedWorker: "
                            "worker "
                            << hexIdentity
                            << " finished with unknown context["
                            << *it << "]" );
#endif
            }
        }
            }

            if ( !removedInProgressRequest )
            {
                // pull in the requesttypecontext for the finished request
                // and remove the inprogress request

                BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                    requestTypeContextLookup.find( type );

                if ( requestTypeAndContext != requestTypeContextLookup.end() )
                {
                    BrokerRequestTypeContext& requestTypeContext =
                        requestTypeAndContext->second;
                    requestTypeContext.RemoveInProgressRequest( id );
                    IncrementStat(
                        requestTypeContext.stats,
                        RequestTypeStat::RequestsCompleted );
                }
                else
                {
                    IncrementStat( stats, BrokerStat::ResponsesForUnknownReqTypes );
                }
            }

#ifdef DEBUG
            clusterLog(
                "Broker.updateContextsForFinishedWorker: sweeping in "
                "other pending requests to "
                << hexIdentity );
#endif

            while ( ( workerInfo->FreeCapacity() > 0 ) && thereAreMoreRequests )
            {
                thereAreMoreRequests = false;

                // it can be but it can still accept work
                // so cycle through the request types until
                // the capacity is reached or
                for ( auto it = workerInfo->GetSupportedRequestTypes().begin();
                    it != workerInfo->GetSupportedRequestTypes().end();
                    ++it )
                {
                    BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                        requestTypeContextLookup.find( *it );

                    clusterLog(
                        "Broker.updateContextsForFinishedWorker: "
                        "sweeping["
                        << *it << "]" );

                    // by this point every req type should be in the context
                    // if( requestTypeAndContext != requestTypeContextLookup.end() )
                    // {
                    BrokerRequestTypeContext& requestTypeContext =
                        requestTypeAndContext->second;

                    RequestState* nextPendingRequest =
                        requestTypeContext.GetNextPendingRequest();

                    if ( nextPendingRequest )
                    {
                        clusterLog(
                            "Broker.updateContextsForFinishedWorker: "
                            "there are more pending requests["
                            << *it << "]" );
                        //will add inprog request to workerInfo and context
                        ret = forwardWorkerParsedClientRequest(
                            workerInfo,
                            requestTypeContext,
                            nextPendingRequest );
                        if ( ret == 0 )
                        {
                            clusterLog(
                                "Broker.updateContextsForFinishedWorker: "
                                "forwarded pending request type["
                                << *it << "]" );

                            if ( workerInfo->FreeCapacity() <= 0 )
                            {
                                // iv) remove it from any pending queues
                                //     it may have been on
                                //     as an optimization we stop at the
                                //     current request type
                                for ( auto it2 =
                                    workerInfo->GetSupportedRequestTypes().begin();
                                    it2 <= it;
                                    ++it2 )
                                {
                                    BrokerRequestTypeContextLookup::iterator
                                        requestTypeAndContext2 =
                                        requestTypeContextLookup.find( *it2 );

                                    if ( requestTypeAndContext2 !=
                                        requestTypeContextLookup.end() )
                                    {
                                        BrokerRequestTypeContext&
                                            requestTypeContext2 =
                                            requestTypeAndContext2->second;

                                        requestTypeContext2.RemoveAvailableWorker(
                                            workerInfo );
                                    }
                                }

                                break;
                            }
                            else
                            {
                                thereAreMoreRequests =
                                    thereAreMoreRequests ||
                                    ( requestTypeContext.GetNextPendingRequest() !=
                                        nullptr );
                            }
                        }
                        else
                        {
#ifdef DEBUG
                            clusterLog(
                                "Broker.updateContextsForFinishedWorker: failed "
                                "sending "
                                << hexIdentity
                                << " pending requests for request type "
                                << *it );
#endif
                            ret = 0;
                    }
                }
                    // }
            }
            }

#ifdef DEBUG
            free( hexIdentity );
#endif
            clusterLog( "Broker.updateContextsForFinishedWorker: "
                "end[" << ret << "|" << workerInfo->FreeCapacity() << "]" );

            return ret;
        }

        int Broker::handleClientRequest( ZMQMessage** lpNewRequest )
        {
            RequestType requestType = 0;
            RequestId requestId = 0;
            ZMQFrameHandler frameHandler;
            ZMQMessage* newRequest = *lpNewRequest;
            ZMQFrame* frame = zmsg_first( newRequest );
            int ret = 0;
            LHCVersionFlags vfs;

            clusterLogSetAction( "Broker.handleClientRequest" );
            clusterLog( "Broker.handleClientRequest: begin" );

            IncrementStat( stats, BrokerStat::RequestsReceived );

            // 1) skip the client envelope
            while ( frame && zframe_size( frame ) != 0 )
            {
                frame = zmsg_next( newRequest );
            }
            if ( frame == nullptr )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleClientRequest: request malformed - missing env delim" );
                IncrementStat( stats, BrokerStat::MalformedClientRequests );
                zmsg_destroy( lpNewRequest );
                return 1;
            }

            // 2) get the version and flags
            frame = zmsg_next( newRequest );
            if ( ( frame == nullptr ) || ( zframe_size( frame ) <= 0 ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleClientRequest: request malformed - missing request type" );
                IncrementStat( stats, BrokerStat::MalformedClientRequests );
                zmsg_destroy( lpNewRequest );
                return 3;
            }
            vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
            // TODO - vfs  clusterLogSetFlags( vfs );

            // 3) move to the request type
            frame = zmsg_next( newRequest );
            if ( ( frame == nullptr ) || ( zframe_size( frame ) <= 0 ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleClientRequest: request malformed - missing request type" );
                IncrementStat( stats, BrokerStat::MalformedClientRequests );
                zmsg_destroy( lpNewRequest );
                return 3;
            }
            requestType = frameHandler.get_frame_value< RequestType >( frame );
            clusterLogSetTag( requestType );

            // 4) move to the request id
            frame = zmsg_next( newRequest );
            if ( ( frame == nullptr ) || ( zframe_size( frame ) <= 0 ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleClientRequest: request malformed - missing request id" );
                IncrementStat( stats, BrokerStat::MalformedClientRequests );
                zmsg_destroy( lpNewRequest );
                return 4;
            }
            requestId = frameHandler.get_frame_value< RequestId >( frame );

            // by this point, newRequest is ready to be sent as is to
            // a worker, (after prepending identity)
            // the request type is known and the request id is known
            ret = forwardParsedClientRequest( vfs, requestType, requestId, lpNewRequest );
            clusterLog( "Broker.handleClientRequest: end[" << ret << "]" );
            return ret;
        }

        int Broker::sendReceipt(
            ZMQMessage** lpSendReceiptMsg, RequestType requestType, RequestId requestId )
        {
            int ret = 0;
            ZMQFrameHandler frameHandler;
            ZMQMessage* sendReceiptMsg = *lpSendReceiptMsg; // just client envelope

            clusterLogSetAction( "Broker.sendReceipt" );
            clusterLog( "Broker.sendReceipt: begin" );

            zmsg_addmem( sendReceiptMsg, NULL, 0 );
            frameHandler.append_message_value< ClusterMessage >(
                sendReceiptMsg,
                ClusterMessage::BrokerSentReceipt );
            frameHandler.append_message_value< RequestType >( sendReceiptMsg, requestType );
            frameHandler.append_message_value< RequestId >( sendReceiptMsg, requestId );

            ret = senderReceiver->Send( lpSendReceiptMsg, frontendMessagingSocket );
            // do not bother cleaning up on failure because there is no compelling reason to atm

            clusterLog( "Broker.sendReceipt: end" );

            return ret;
        }

        int Broker::handleCopiedRequest(
            RequestState* requestState,
            const LHCVersionFlags& vfs,
            RequestType requestType,
            RequestId requestId,
            ZMQMessage** lpCopiedRequest )
        {
            int ret = 0;

            clusterLogSetAction( "Broker.handleCopiedRequest" )
                clusterLog( "Broker.handleCopiedRequest: begin" );

            if ( vfs.hasAny( MessageFlag::ReceiptOnSend ) )
            {
                ZMQMessage* copiedForReceipt = nullptr;
                LHCVersionFlags vfsMinusReceiptOnSend(
                    vfs.version,
                    vfs.flags & ( ~MessageFlag::ReceiptOnSend ) );

                clusterLog( "Broker.handleCopiedRequest: sending send receipt" );

                if ( vfs.hasAny( MessageFlag::RetryOnDeath ) ) // lpCopiedRequest is whole message
                {
                    copiedForReceipt = zmsg_new();
                    ZMQMessageEnvCopy( copiedForReceipt, *lpCopiedRequest );

                    ret = sendReceipt( &copiedForReceipt, requestType, requestId );
                    if ( copiedForReceipt )
                        zmsg_destroy( &copiedForReceipt );

                    requestState->UpdateRequest( lpCopiedRequest, vfsMinusReceiptOnSend );
                }
                else if ( vfs.hasAny( MessageFlag::NotifyOnRedirect | MessageFlag::NotifyOnDeath ) )
                {
                    copiedForReceipt = zmsg_dup( *lpCopiedRequest );

                    ret = sendReceipt( &copiedForReceipt, requestType, requestId );
                    if ( copiedForReceipt )
                        zmsg_destroy( &copiedForReceipt );

                    requestState->UpdateRequest( lpCopiedRequest, vfsMinusReceiptOnSend );
                }
                else
                {
                    ret = sendReceipt( lpCopiedRequest, requestType, requestId );
                    if ( *lpCopiedRequest )
                        zmsg_destroy( lpCopiedRequest );
                    requestState->UpdateRequest( vfsMinusReceiptOnSend );
                }
            }
            else
            {
                clusterLog( "Broker.handleCopiedRequest: nothing to send" );
                requestState->UpdateRequest( lpCopiedRequest );
            }

            clusterLog( "Broker.handleCopiedRequest: end" );

            return ret;
        }

        ZMQMessage* Broker::copyRequestForFlags( const LHCVersionFlags& vfs, ZMQMessage* newRequest )
        {
            ZMQMessage* copiedNewRequest = nullptr;

            if ( vfs.hasAny( MessageFlag::RetryOnDeath ) )
            {
                copiedNewRequest = zmsg_dup( newRequest );
            }
            else if ( vfs.hasAny(
                MessageFlag::NotifyOnRedirect |
                MessageFlag::NotifyOnDeath |
                MessageFlag::ReceiptOnSend ) )
            {
                copiedNewRequest = zmsg_new();
                if ( copiedNewRequest )
                    ZMQMessageEnvCopy( copiedNewRequest, newRequest );
            }

            return copiedNewRequest;
        }

        int Broker::notifyOnRedirect( RequestType requestType, RequestState* requestState )
        {
            clusterLogSetAction( "Broker.notifyOnRedirect" )
                clusterLog( "Broker.notifyOnRedirect: begin" );

            ZMQMessage* redirectNotification = zmsg_new();
            ZMQMessage* requestMessage = *( requestState->GetMessage() );
            ZMQFrameHandler frameHandler;
            int ret = 0;

            if ( requestMessage == nullptr )
                return 1;

            ZMQMessageEnvCopy( redirectNotification, requestMessage );
            zmsg_addmem( redirectNotification, NULL, 0 );
            frameHandler.append_message_value< ClusterMessage >(
                redirectNotification,
                ClusterMessage::BrokerResponse );
            frameHandler.append_message_value< LHCVersionFlags >(
                redirectNotification,
                requestState->GetVersionAndFlags() );
            frameHandler.append_message_value< RequestType >(
                redirectNotification,
                requestType );
            frameHandler.append_message_value< RequestId >(
                redirectNotification,
                requestState->GetRequestId() );
            frameHandler.append_message_value< RequestStatus >(
                redirectNotification,
                RequestStatus::Redirected );

            ret = senderReceiver->Send( &redirectNotification, frontendMessagingSocket );
            if ( ret && redirectNotification )
                zmsg_destroy( &redirectNotification );

            clusterLog( "Broker.notifyOnRedirect: end" );

            return ret;
        }

        int Broker::rejectRequest(
            const LHCVersionFlags& vfs,
            RequestType requestType,
            RequestId requestId,
            ZMQMessage** lpNewRequest,
            RequestStatus requestStatus )
        {
            ZMQMessage* newRequest = *lpNewRequest;
            ZMQFrameHandler frameHandler;
            int ret = 0;

            clusterLogSetAction( "Broker.rejectRequest1" )
                clusterLog( "Broker.rejectRequest1: begin" );

            // add vfs
            frameHandler.append_message_value< LHCVersionFlags >( newRequest, vfs );
            // add type
            frameHandler.append_message_value< RequestType >( newRequest, requestType );
            // add id
            frameHandler.append_message_value< RequestId >( newRequest, requestId );
            // add reject status
            frameHandler.append_message_value< RequestStatus >( newRequest, requestStatus );

            ret = senderReceiver->Send( lpNewRequest, frontendMessagingSocket );
            if ( ret && *lpNewRequest )
                zmsg_destroy( lpNewRequest );

            clusterLog( "Broker.rejectRequest1: end" );

            return ret;
        }

        int Broker::rejectRequest(
            ZMQMessage** lpNewRequest,
            RequestStatus requestStatus )
        {
            ZMQFrame* frame = nullptr;
            ZMQMessage* newRequest = *lpNewRequest;
            ZMQFrameHandler frameHandler;
            int ret = 0;

            clusterLogSetAction( "Broker.rejectRequest2" )
                clusterLog( "Broker.rejectRequest2: begin" );

            // skip env, assuming msg has already been validated
            frame = zmsg_first( newRequest );
            while ( zframe_size( frame ) )
                frame = zmsg_next( newRequest );

            // version and flags
            frame = zmsg_next( newRequest );
            // request type
            frame = zmsg_next( newRequest );
            // request id
            frame = zmsg_next( newRequest );

            // remove data
            while ( ( frame = zmsg_next( newRequest ) ) != nullptr )
                zmsg_remove( newRequest, frame );

            // add reject status
            frameHandler.append_message_value< RequestStatus >( newRequest, requestStatus );

            ret = senderReceiver->Send( lpNewRequest, frontendMessagingSocket );
            if ( ret && *lpNewRequest )
                zmsg_destroy( lpNewRequest );

            clusterLog( "Broker.rejectRequest2: end" );

            return ret;
        }

        int Broker::forwardParsedClientRequest(
            const LHCVersionFlags& vfs,
            RequestType type,
            RequestId id,
            ZMQMessage** lpParsedNewRequest )
        {
            int ret = 0;
            ZMQFrameHandler frameHandler;

            clusterLogSetAction( "Broker.forwardParsedClientRequest" );
            clusterLog( "Broker.forwardParsedClientRequest: begin" );

            BrokerRequestTypeContextLookup::iterator requestTypeAndContext =
                requestTypeContextLookup.find( type );

            if ( requestTypeAndContext != requestTypeContextLookup.end() )
            {
                BrokerRequestTypeContext& requestTypeContext =
                    requestTypeAndContext->second;
                RequestState* requestState =
                    requestTypeContext.GetRequestState( id );

                if ( requestState )
                {
                    if ( requestState->GetVersionAndFlags().hasAny( MessageFlag::AllowRedirect ) )
                    {
                        if ( requestState->IsInProgress() )
                        {
                            WorkerInfo* workerInfo = requestState->GetAssignedWorker();
                            ZMQMessage* copiedNewRequest = nullptr;
                            IncrementStat( stats, BrokerStat::ClientRedirectsReceived );

                            clusterLog( "Broker.forwardParsedClientRequest: redirecting"
                                << "[" << type << "|" << id << "]" );

                            copiedNewRequest = copyRequestForFlags( vfs, *lpParsedNewRequest );

                            ZMQFrame* frame = zframe_dup(
                                const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );

                            frameHandler.prepend_message_value< ClusterMessage >(
                                *lpParsedNewRequest,
                                ClusterMessage::BrokerNewRequest );
                            zmsg_pushmem( *lpParsedNewRequest, NULL, 0 );
                            zmsg_prepend( *lpParsedNewRequest, &frame );

                            ret = senderReceiver->Send( lpParsedNewRequest, backendMessagingSocket );
                            if ( ret == 0 )
                            {
                                IncrementStat( stats, BrokerStat::WorkerRedirectsSent );

                                if ( requestState->GetVersionAndFlags().hasAny(
                                    MessageFlag::NotifyOnRedirect ) )
                                {
                                    ret = notifyOnRedirect( type, requestState );
                                    checkRet( ret, 0, "Broker.forwardParsedClientRequest: "
                                        "failed to send redirect notification" );
                                }

                                if ( copiedNewRequest )
                                {
                                    ret = handleCopiedRequest(
                                        requestState, vfs, type, id, &copiedNewRequest );
                                    checkRet( ret, 0, "Broker.forwardParsedClientRequest: "
                                        "failed to handle copied request" );
                                }
                                else if ( *( requestState->GetMessage() ) )
                                {
                                    zmsg_destroy( requestState->GetMessage() );
                                }

                                requestState->UpdateRequest( vfs );
                            }
                            else
                            {
                                clusterLogWithSeverity(
                                    LHClusterNS::SeverityLevel::Error,
                                    "Broker.forwardParsedClientRequest: failed "
                                    "to send redirect" );
                                IncrementStat( stats, BrokerStat::WorkerRedirectsSentFailed );
                                ret = 0;
                                zmsg_destroy( lpParsedNewRequest );
                                // TODO - failure to lpParsedNewRequest, FatalError
                                //        or buffer to retry
                            }
                        }
                        else // assert requestState->IsPending()
                        {
                            IncrementStat( stats, BrokerStat::ClientDuplicateRequests );

                            if ( requestState->GetVersionAndFlags().hasAny(
                                MessageFlag::NotifyOnRedirect ) )
                            {
                                ret = notifyOnRedirect( type, requestState );
                                checkRet( ret, 0, "failed to send redirect notification" );
                                requestState->SetRequest( lpParsedNewRequest, vfs );
                            }
                            else
                            {
                                requestState->UpdateRequest( lpParsedNewRequest, vfs );
                            }
                        }
                    }
                    else
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Info,
                            "rejecting message already requested" );
                        ret = rejectRequest( lpParsedNewRequest, RequestStatus::Rejected );
                        checkRet( ret, 0, "failed to send reject due to already requested" );
                    }
                }
                else    // TODO - some of this into subroutine
                {
                    IncrementStat( requestTypeContext.stats,
                        RequestTypeStat::RequestsReceivedFor );
                    WorkerInfo* workerInfo = requestTypeContext.GetNextAvailableWorker();

                    if ( workerInfo )
                    {
                        ret = forwardWorkerParsedClientRequest(
                            workerInfo, id, vfs, lpParsedNewRequest, requestTypeContext );

                        if ( ret != 0 )
                        {
#ifdef DEBUG
                            char* hexIdentity = zframe_strhex(
                                const_cast<ZMQFrame*>(
                                    workerInfo->GetIdentity() ) );
                            clusterLogWithSeverity(
                                LHClusterNS::SeverityLevel::Error,
                                "Broker.forwardParsedClientRequest: "
                                "failed sending to "
                                << hexIdentity << " - adding as pending"
                                << "[" << type << "|" << id << "]" );
                            free( hexIdentity );
#else
                            ret = 0;
#endif
                            requestTypeContext.AddPendingRequest( id, vfs, lpParsedNewRequest );
                    }
                        else
                        {
                            if ( workerInfo->FreeCapacity() <= 0 )
                            {
                                for ( auto it2 = workerInfo->GetSupportedRequestTypes().begin();
                                    it2 != workerInfo->GetSupportedRequestTypes().end();
                                    ++it2 )
                                {
                                    BrokerRequestTypeContextLookup::iterator
                                        requestTypeAndContext2 =
                                        requestTypeContextLookup.find( *it2 );

                                    if ( requestTypeAndContext2 !=
                                        requestTypeContextLookup.end() )
                                    {
                                        BrokerRequestTypeContext& requestTypeContext2 =
                                            requestTypeAndContext2->second;

                                        requestTypeContext2.RemoveAvailableWorker(
                                            workerInfo );
                                    }
                                }

                            }
                        }
                }
                    else
                    {
                        clusterLog(
                            "Broker.forwardParsedClientRequest: "
                            "adding pending request2["
                            << type << "|" << id << "]" );
                        requestTypeContext.AddPendingRequest( id, vfs, lpParsedNewRequest );
                    }
            }
        }
            else    //not found, create context for the request
            {
                std::pair< BrokerRequestTypeContextLookup::iterator, bool >
                    insertedValue( requestTypeContextLookup.insert(
                        std::move(
                            BrokerRequestTypeContextLookup::value_type(
                                type,
                                BrokerRequestTypeContext( type ) ) ) ) );

                clusterLog(
                    "Broker.forwardParsedClientRequest: adding pending request1["
                    << type << "|" << id << "]" );

                insertedValue.first->second.AddPendingRequest(
                    id,
                    vfs,
                    lpParsedNewRequest );

                IncrementStat( stats, BrokerStat::RequestTypesAdded );
                IncrementStat( insertedValue.first->second.stats,
                    RequestTypeStat::RequestsReceivedFor );
            }

            clusterLog( "Broker.forwardParsedClientRequest: end[" << ret << "]" );

            return ret;
        }

        int Broker::forwardWorkerParsedClientRequest(
            WorkerInfo* workerInfo,
            BrokerRequestTypeContext& requestTypeContext,
            RequestState* requestState )
        {
            ZMQMessage** lpNewRequest = requestState->GetMessage();
            int ret = 0;
            ZMQFrame* frame = nullptr;
            RequestType requestType = requestTypeContext.GetRequestType();
            RequestId requestId = requestState->GetRequestId();
            ZMQFrameHandler frameHandler;
            ZMQMessage* copiedRequest = nullptr;

            clusterLogSetAction( "Broker.forwardWorkerParsedClientRequest1" )

#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
            clusterLog( "Broker.forwardWorkerParsedClientRequest1: begin "
                << hexIdentity );
#endif
            copiedRequest = copyRequestForFlags( requestState->GetVersionAndFlags(), *lpNewRequest );

            frameHandler.prepend_message_value< ClusterMessage >(
                *lpNewRequest,
                ClusterMessage::BrokerNewRequest );
            zmsg_pushmem( *lpNewRequest, NULL, 0 );
            frame = zframe_dup( const_cast<ZMQFrame*>(
                workerInfo->GetIdentity() ) );
            zmsg_prepend( *lpNewRequest, &frame );

            ret = senderReceiver->Send( lpNewRequest, backendMessagingSocket );
            if ( ret == 0 )
            {
                IncrementStat( stats, BrokerStat::RequestsSent );
                workerInfo->AddInProgressRequest( requestType, requestState );
                requestTypeContext.AssignWorkerFromPending( requestState, workerInfo );
                workerInfo->UpdateBrokerLastActive( currentTime );  // suppress redundant heartbeat
#ifdef DEBUG
                clusterLog(
                    "Broker.forwardWorkerParsedClientRequest1: success "
                    << hexIdentity << "[" << requestType << "|" << requestId << "]" );
#endif
                if ( copiedRequest )
                {
                    ret = handleCopiedRequest(
                        requestState,
                        requestState->GetVersionAndFlags(),
                        requestType,
                        requestId,
                        &copiedRequest );
                    checkRet( ret, 0, "Broker.forwardWorkerParsedClientRequest1: "
                        "failed to handle copied request" );
                }
            }
            else
            {
                IncrementStat( stats, BrokerStat::RequestsSentFailed );
                //undo the changes we made identity | null | brokermessage
                frame = zmsg_pop( *lpNewRequest );
                zframe_destroy( &frame );
                frame = zmsg_pop( *lpNewRequest );
                zframe_destroy( &frame );
                frame = zmsg_pop( *lpNewRequest );
                zframe_destroy( &frame );
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.forwardWorkerParsedClientRequest1: failed "
                    << hexIdentity << "[" << requestType << "|" << requestId << "]" );
#endif
            }

#ifdef DEBUG
            clusterLog(
                "Broker.forwardWorkerParsedClientRequest1: end " << hexIdentity
                << hexIdentity << "[" << requestType << "|" << requestId << "|"
                << ret << "]" );
            free( hexIdentity );
#endif
            return ret;
        }



        int Broker::forwardWorkerParsedClientRequest(
            WorkerInfo* workerInfo,
            RequestId requestId,
            const LHCVersionFlags& vfs,
            ZMQMessage** lpNewRequest,
            BrokerRequestTypeContext& requestTypeContext )
        {
            ZMQMessage* newRequest = *lpNewRequest;
            int ret = 0;
            ZMQFrame* frame = nullptr;
            RequestType requestType = requestTypeContext.GetRequestType();
            ZMQFrameHandler frameHandler;
            ZMQMessage* copiedNewRequest = nullptr;

            clusterLogSetAction( "Broker.forwardWorkerParsedClientRequest2" )
#ifdef DEBUG
                char* hexIdentity = zframe_strhex(
                    const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
            clusterLog(
                "Broker.forwardWorkerParsedClientRequest2: begin "
                << hexIdentity << "[" << requestType << "|" << requestId << "]" );
#endif

            copiedNewRequest = copyRequestForFlags( vfs, *lpNewRequest );

            frameHandler.prepend_message_value< ClusterMessage >(
                newRequest,
                ClusterMessage::BrokerNewRequest );
            zmsg_pushmem( newRequest, NULL, 0 );
            frame = zframe_dup(
                const_cast<ZMQFrame*>( workerInfo->GetIdentity() ) );
            zmsg_prepend( newRequest, &frame );

            ret = senderReceiver->Send( lpNewRequest, backendMessagingSocket );
            if ( ret == 0 )
            {
                IncrementStat( stats, BrokerStat::RequestsSent );
                RequestState* requestState(
                    requestTypeContext.AddInProgressRequest( requestId, vfs, workerInfo ) );
                workerInfo->AddInProgressRequest( requestType, requestState );
                workerInfo->UpdateWorkerLastActive( currentTime );
#ifdef DEBUG
                clusterLog(
                    "Broker.forwardWorkerParsedClientRequest2: success "
                    << hexIdentity << "[" << requestType << "|" << requestId << "]" );
#endif

                if ( copiedNewRequest )
                {
                    ret = handleCopiedRequest(
                        requestState, vfs, requestType, requestId, &copiedNewRequest );
                    checkRet( ret, 0, "Broker.forwardWorkerParsedClientRequest2: "
                        "failed to handle copied request" );
                }
            }
            else
            {
                IncrementStat( stats, BrokerStat::RequestsSentFailed );
                //undo the changes we made
                frame = zmsg_pop( newRequest );
                zframe_destroy( &frame );
                frame = zmsg_pop( newRequest );
                zframe_destroy( &frame );
                frame = zmsg_pop( newRequest );
                zframe_destroy( &frame );
#ifdef DEBUG
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.forwardWorkerParsedClientRequest2: failed "
                    << hexIdentity << "[" << requestType << "|" << requestId << "]" );
#endif
            }

#ifdef DEBUG
            clusterLog(
                "Broker.forwardWorkerParsedClientRequest2: end "
                << hexIdentity << "[" << requestType << "|" << requestId << "|"
                << ret << "]" );
            free( hexIdentity );
#endif
            return ret;
        }

        int Broker::handleClientHeartbeat( ZMQMessage** lpHeartbeatMsg )
        {
            int ret = 0;
            ZMQMessage* heartbeatMsg = *lpHeartbeatMsg;
            ZMQFrameHandler frameHandler;
            ZMQFrame* frame = nullptr;
            ZMQFrame* identity = nullptr;

            clusterLogSetAction( "Broker.handleClientHeartbeat" )
                clusterLog( "Broker.handleClientHeartbeat: begin" )

                IncrementStat( stats, BrokerStat::ClientHeartbeatsReceived );

            // Heartbeats from/to clients, at this time, are different in
            // that the broker to client heartbeats are only sent
            // "on demand" when a client heartbeat is received. The broker does
            // not, at this time, keep track of clients
            // and does not independently heartbeat them
            // Still because I am lazy, I sent most of the same
            // information in the client/broker heartbeats as I do in the
            // worker/broker heartbeats heartbeats

            // Remove everything from the first identity
            // Only support sending to the closest client at the moment
            identity = zmsg_pop( heartbeatMsg );
            while ( ( frame = zmsg_pop( heartbeatMsg ) ) != nullptr )
            {
                zframe_destroy( &frame );
            }

            zmsg_prepend( heartbeatMsg, &identity );

            ret = SendHeartbeat(
                frontendMessagingSocket,
                currentTime,
                selfHeartbeatDelay,
                static_cast<std::uint32_t>( ClusterMessage::BrokerHeartbeat ),
                lpHeartbeatMsg,
                *senderReceiver );
            if ( ret == 0 )
            {
                IncrementStat( stats, BrokerStat::ClientHeartbeatsSent );
            }
            else
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleClientHeartbeat: failed to send heartbeat" )
                    IncrementStat( stats, BrokerStat::ClientHeartbeatsSentFailed );
                zmsg_destroy( lpHeartbeatMsg );
            }

            clusterLog( "Broker.handleClientHeartbeat: end" )

                return ret;
        }

        int Broker::handleWorkerReceipt( ZMQFrame* identityFrame, ZMQMessage** lpReceiptMsg )
        {
            ZMQFrameHandler frameHandler;
            int ret = 0;
            WorkerInfo* workerInfo = nullptr;

            IncrementStat( stats, BrokerStat::WorkerReceiptsReceived );

            clusterLogSetAction( "Broker.handleWorkerReceipt" );
            clusterLog( "Broker.handleWorkerReceipt: begin" );

            workerInfo = workerInfoLookup.LookupWorkerInfo( identityFrame );
            if ( workerInfo != nullptr )
                workerInfo->UpdateWorkerLastActive( currentTime );

            // All we want to do is itnerject the ClusterMessage type after
            // the client envelope
            //TODO - note that there is no easy way to insert a frame into
            //       the middle of a message so
            //       as an optimization I add the broker message type when
            //       forming the receipt within the Worker class
            //frameHandler.prepend_message_value< ClusterMessage >(
            //  *lpReceiptMsg, ClusterMessage::BrokerReceipt );

            ret = senderReceiver->Send( lpReceiptMsg, frontendMessagingSocket );
            if ( ret == 0 )
            {
                IncrementStat( stats, BrokerStat::WorkerReceiptsSent );
            }
            else
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Broker.handleWorkerReceipt: failed to send worker receipt" )
                    IncrementStat( stats, BrokerStat::WorkerReceiptsSentFailed );
                zmsg_destroy( lpReceiptMsg );
            }

            clusterLog( "Broker.handleWorkerReceipt: end" );

            return ret;
        }

#ifdef STATS
        int Broker::dumpStatsHeaders()
        {
            {
                clusterLogSetAction( "Stats.Broker" )
                    clusterLogToChannel(
                        LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                        MessageStats< BrokerStat >::GetDelimitedStatNames() )
            }

            {
                clusterLogSetAction( "Stats.RequestTypeBroker" );
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    MessageStats< RequestTypeStat >::GetDelimitedStatNames() )
            }

            return 0;
        }

        int Broker::dumpStats()
        {
            {
                clusterLogSetAction( "Stats.Broker" )
                    clusterLogToChannel(
                        LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                        stats.GetDelimitedStatValues() )
                    ClearStats( stats );
            }

            for ( auto it = requestTypeContextLookup.begin();
                it != requestTypeContextLookup.end();
                ++it )
            {
                clusterLogSetAction( "Stats.RequestTypeBroker" );
                clusterLogSetTag( it->first );
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    it->second.stats.GetDelimitedStatValues() )
                    ClearStats( it->second.stats );
            }

            return 0;
        }
#endif
    }
}
