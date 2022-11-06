#include <lhcluster/cluster.h>
#include <lhcluster_impl/worker.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <chrono>
#include <thread>

namespace LHClusterNS
{
    namespace Impl
    {
#ifdef STATS
        int dumpStatsWorkerReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpWorker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Worker*)lpWorker )->updateCurrentTime();
                return ( (Worker*)lpWorker )->dumpStats();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Worker.dumpStatsWorkerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.dumpStatsWorkerReactor: uncaught exception[ "
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
                    clusterLogSetAction( "Worker.dumpStatsWorkerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.dumpStatsWorkerReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }
#endif

        int requestProcessorReactor(
            ZMQReactor* reactor,
            SocketPair* handlerSocket,
            void* lpWorker )
        {
            (void)reactor;
            (void)handlerSocket;
            try
            {
                return ( (Worker*)lpWorker )->handleProcessorMessage();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Worker.requestProcessorReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.requestProcessorReactor: uncaught exception[ "
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
                    clusterLogSetAction( "Worker.requestProcessorReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.requestProcessorReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }

        int heartbeatBrokerReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpWorker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Worker*)lpWorker )->updateCurrentTime();
                return ( (Worker*)lpWorker )->sendBrokerHeartbeat();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Worker.heartbeatBrokerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.heartbeatBrokerReactor: uncaught exception[ "
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
                    clusterLogSetAction( "Worker.heartbeatBrokerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.heartbeatBrokerReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }

        int brokerMessagingReactor(
            ZMQReactor* reactor,
            SocketDealer* dealerSocket,
            void* lpWorker )
        {
            (void)reactor;
            (void)dealerSocket;
            try
            {
                return ( (Worker*)lpWorker )->handleBrokerMessage();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLog( "Worker.brokerMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.brokerMessagingReactor: uncaught exception[ "
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
                    clusterLog( "Worker.brokerMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.brokerMessagingReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        int workerPrimaryControllerReactor(
            ZMQReactor* reactor,
            SocketPair* listenerPairSocket,
            void* lpWorker )
        {
            (void)reactor;
            (void)listenerPairSocket;
            Worker* worker = (Worker*)lpWorker;
            int ret = -1;

            try
            {
                // TODO - more commands here
                clusterLogSetAction( "Worker.workerPrimaryControllerReactor" );
                clusterLog(
                    "Worker.workerPrimaryControllerReactor: start" );
                worker->setExitState( WorkerExitState::GracefulStop );
                clusterLog(
                    "Worker.workerPrimaryControllerReactor: end" );

            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Worker.workerPrimaryControllerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.workerPrimaryControllerReactor: uncaught exception[ "
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
                    clusterLogSetAction( "Worker.workerPrimaryControllerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.workerPrimaryControllerReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }


            return ret;
        }

        int checkPulseBrokerReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpWorker )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                ( (Worker*)lpWorker )->updateCurrentTime();
                return ( (Worker*)lpWorker )->checkBrokerPulse();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "Worker.checkPulseBrokerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.checkPulseBrokerReactor: uncaught exception[ "
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
                    clusterLogSetAction( "Worker.checkPulseBrokerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "Worker.checkPulseBrokerReactor: uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        const char* WorkerConstructionFailed::what() const throw( )
        {
            return "Failed to construct worker";
        }

        void Worker::setExitState( WorkerExitState newExitState )
        {
            exitState = newExitState;
        }

        Worker::Worker( const Endpoint& _selfMessagingEndpoint,
            const Endpoint& _brokerMessagingEndpoint,
            const Endpoint& _selfControllerEndpoint,
            Delay _selfHeartbeatDelay,
            Delay _initialBrokerHeartbeatDelay,
            Delay _brokerPulseDelay,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >  _senderReceiver,
            const Endpoint& _requestProcessorMessagingEndpoint,
            const Endpoint& _requestProcessorControllerEndpoint,
            std::unique_ptr< IRequestHandler > _requestHandler,
            std::unique_ptr< IZMQSocketFactory > _requestProcessorSocketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver > _requestProcessorSenderReceiver )
            : IWorker()
            , capacity( 1 )
            , exitState( WorkerExitState::None )
            // Worker's address
            , selfMessagingEndpoint( _selfMessagingEndpoint )
            // Broker's address
            , brokerMessagingEndpoint( _brokerMessagingEndpoint )
            // Address of local pair socket
            , selfControllerEndpoint( _selfControllerEndpoint )
            // Worker's dealer socket for sending/receiving
            , messagingSocket( _socketFactory->Create( SocketType::Dealer ) )
            // The Pair socket stop commands are received on
            , listenerSocket( _socketFactory->Create( SocketType::Pair ) )
            // The Pair socket stop commands are sent through
            , signalerSocket( _socketFactory->Create( SocketType::Pair ) )
            , requestProcessorMessagingSocket( _socketFactory->Create( SocketType::Pair ) )
            // Reactor for handling messages, sending HBs, pulse
            , reactor( zloop_new() )
            // Numeric ID of reactor heartbeat callback
            , heartbeatBrokerReactorId( -1 )
            // Numeric ID of reactor check broker callback
            , checkPulseBrokerReactorId( -1 )
            // Numeric ID of reactor dump stats callback
#ifdef STATS
            , dumpStatsReactorId( -1 )
#endif
            // Secondary thread where the reactor runs
            , reactorThread( nullptr )
            // Delay between sending heartbeats to broker
            , brokerHeartbeatDelay( _initialBrokerHeartbeatDelay )
            // Delay between receiving heartbeats from broker
            , selfHeartbeatDelay( _selfHeartbeatDelay )
            // Delay between checking a broker's pulse
            , brokerPulseDelay( _brokerPulseDelay )
            // The length of inactivity required to kill broker
            , brokerDeathDelay( 2 * _selfHeartbeatDelay )
            // When the broker was last active
            , brokerLastActive( 0 )
            // When the worker last sent activity to broker
            , workerLastActive( 0 )
            // Thread that the RequestProcessor runs in
            , requestProcessor(
                _requestProcessorMessagingEndpoint,
                _requestProcessorControllerEndpoint,
                move( _requestProcessorSocketFactory ),
                move( _requestProcessorSenderReceiver ),
                move( _requestHandler ) )
#ifdef STATS
            , stats()
#endif
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
        {
            if ( !( messagingSocket &&
                reactor &&
                listenerSocket &&
                signalerSocket &&
                requestProcessorMessagingSocket &&
                requestProcessor.SupportedRequestTypes().size() ) )
            {
                throw WorkerConstructionFailed();
            }

            //initialize the request type contexts
            initializeRequestTypeContexts();
        }

        void Worker::initializeRequestTypeContexts()
        {
            const std::vector< RequestType > supportedRequestTypes(
                requestProcessor.SupportedRequestTypes() );

            for ( auto it = supportedRequestTypes.begin();
                it != supportedRequestTypes.end();
                ++it )
            {
                requestTypeContexts.insert(
                    std::pair< RequestType, WorkerRequestTypeContext >(
                        *it,
                        WorkerRequestTypeContext( *it ) ) );
            }
        }

        Worker::~Worker()
        {
            try
            {
                Stop();

                if ( reactor )
                {
                    zloop_destroy( &reactor );
                }

                if ( requestProcessorMessagingSocket )
                {
                    socketFactory->Destroy( &requestProcessorMessagingSocket );
                }

                if ( messagingSocket )
                {
                    socketFactory->Destroy( &messagingSocket );
                }

                if ( signalerSocket )
                {
                    socketFactory->Destroy( &signalerSocket );
                }

                if ( listenerSocket )
                {
                    socketFactory->Destroy( &listenerSocket );
                }
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "Worker.destructor" );
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Fatal,
                        "Worker.destructor: exception thrown in destructor" );
                }
                catch ( ... )
                {
                }
            }
        }

        void Worker::Stop()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() );
            clusterLogSetAction( "Worker.stop" );
            clusterLog( "Worker.stop: begin" );

            if ( reactorThread )
            {
                ret = senderReceiver->Signal( signalerSocket, 0 );
                checkRet( ret, 0, "Worker.stop: failed signal stop" );
                if ( reactorThread->joinable() )
                {
                    clusterLog( "Worker.stop: joining" );
                    reactorThread->join();
                }
#ifdef DEBUG
                else
                    clusterLog( "Worker.stop: not joining" );
#endif

                reactorThread = nullptr;
            }

            ret = teardownControllerCallbacks();
            checkRet( ret, 0, "Worker.stop: failed to td controller cbs" );

            ret = deactivateControllerSockets();
            checkRet( ret, 0, "Worker.stop: failed to deactivate controller sockets" );

            clusterLog( "Worker.stop: end" );
        }

        int Worker::activateControllerSockets()
        {
            int ret = 0;

            //Connect the listener and signaler
            ret = socketFactory->Bind( listenerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Worker.activateControllerSockets: failed to bind controller["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            ret += socketFactory->Connect( signalerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Worker.activateControllerSockets: failed to connect controller["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            return ret;
        }

        int Worker::activateMessagingSockets()
        {
            int ret = 0;

            //Connect the dealer to the broker
            ret = socketFactory->Bind( messagingSocket, selfMessagingEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Worker.activateMessagingSockets: failed to bind self["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            ret += socketFactory->Connect( messagingSocket, brokerMessagingEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Worker.activateMessagingSockets: failed to connect broker["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            ret += socketFactory->Connect(
                requestProcessorMessagingSocket, requestProcessor.selfMessagingEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "Worker.activateMessagingSockets: failed to connect request processor["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );


            updateBrokerLastActive( currentTime );

            return ret;
        }

        int Worker::setupProcessorCallbacks()
        {
            int ret = 0;
            ret = zloop_reader(
                reactor,
                requestProcessorMessagingSocket,
                requestProcessorReactor,
                this );
            return ret;
        }

        int Worker::teardownControllerCallbacks()
        {
            int ret = 0;

            clusterLog( "Worker.teardownControllerCallbacks: start" );

            zloop_reader_end( reactor, listenerSocket );

            clusterLog( "Worker.teardownControllerCallbacks: end" );

            return ret;
        }

        int Worker::setupControllerCallbacks()
        {
            int ret = 0;

            ret = zloop_reader(
                reactor,
                listenerSocket,
                workerPrimaryControllerReactor,
                this );

            return ret;
        }

        int Worker::setupMessagingCallbacks()
        {
            int ret = 0;
            std::chrono::milliseconds heartbeatSendDelayMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    brokerHeartbeatDelay );
            std::chrono::milliseconds pulseCheckDelayMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    brokerPulseDelay );
#ifdef STATS
            std::chrono::milliseconds dumpStatsDelay_ms =
                std::chrono::milliseconds( 5000 );
#endif

            // Setup reactor callback functions,
            // message handler
            ret = zloop_reader( reactor,
                messagingSocket,
                brokerMessagingReactor,
                this );
            // check pulse of broker
            checkPulseBrokerReactorId = zloop_timer(
                reactor,
                pulseCheckDelayMs.count(),
                0,
                checkPulseBrokerReactor,
                this );
            // send heartbeat to broker
            heartbeatBrokerReactorId = zloop_timer(
                reactor,
                heartbeatSendDelayMs.count(),
                0,
                heartbeatBrokerReactor,
                this );

#ifdef STATS
            dumpStatsReactorId =
                zloop_timer(
                    reactor,
                    dumpStatsDelay_ms.count(),
                    0,
                    dumpStatsWorkerReactor,
                    this );
#endif


            return ret;
        }

        int Worker::teardownProcessorCallbacks()
        {
            int ret = 0;
            zloop_reader_end( reactor, requestProcessorMessagingSocket );
            return ret;
        }

        int Worker::teardownMessagingCallbacks()
        {
            int ret = 0;

            // Setup reactor callback functions, callback
            // functions must be friends of the worker class
            // message handler
            zloop_reader_end( reactor, messagingSocket );
            // heartbeat broker
            zloop_timer_end( reactor, heartbeatBrokerReactorId );
            // check pulse of broker
            zloop_timer_end( reactor, checkPulseBrokerReactorId );
#ifdef STATS
            // dump stats
            zloop_timer_end( reactor, dumpStatsReactorId );
#endif

            return ret;
        }

        int Worker::deactivateMessagingSockets()
        {
            int ret = 0;

            //disconnect dealer socket from broker and unbind dealer socket
            ret = socketFactory->DisConnect( messagingSocket, brokerMessagingEndpoint.path() );
            ret += socketFactory->UnBind( messagingSocket, selfMessagingEndpoint.path() );

            ret += socketFactory->DisConnect(
                requestProcessorMessagingSocket, selfMessagingEndpoint.path() );

            ret += RefreshSocket(
                *socketFactory,
                &requestProcessorMessagingSocket, SocketType::Pair );

            return ret;
        }

        int Worker::deactivateControllerSockets()
        {
            int ret = 0;

            clusterLogSetAction( "Worker.deactivateControllerSockets" );
            clusterLog( "Worker.deactivateControllerSockets: start" );

            //disconnect and unbind pairs
            clusterLog( "Worker.deactivateControllerSockets: disconnect signaler" );

            ret = socketFactory->DisConnect( signalerSocket, selfControllerEndpoint.path() );
            checkRet( ret, 0,
                "Worker.deactivateControllerSockets: "
                "failed to disconnect signaler" );

            clusterLog( "Worker.deactivateControllerSockets: disconnect listener" );
            ret += socketFactory->UnBind( listenerSocket, selfControllerEndpoint.path() );
            checkRet( ret, 0,
                "Worker.deactivateControllerSockets: "
                "failed to disconnect listener" );

            clusterLog( "Worker.deactivateControllerSockets: refresh signaler" );
            ret += RefreshSocket(
                *socketFactory,
                &signalerSocket, SocketType::Pair );
            checkRet( ret, 0,
                "Worker.deactivateControllerSockets: "
                "failed to refresh signaler" );

            clusterLog( "Worker.deactivateControllerSockets: refresh listener" );
            ret += RefreshSocket(
                *socketFactory,
                &listenerSocket, SocketType::Pair );
            checkRet( ret, 0,
                "Worker.deactivateControllerSockets: "
                "failed to refresh listener" );

            clusterLog( "Worker.deactivateControllerSockets: end" );

            return ret;
        }

        void Worker::Start()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() );
            clusterLogSetAction( "Worker.start" );
            clusterLog( "Worker.start: begin" );

            clusterLog( "Worker.start: starting with parameters\n" <<
                "    selfMessagingEndpoint[" << selfMessagingEndpoint << "]\n" <<
                "    brokerMessagingEndpoint[" << brokerMessagingEndpoint << "]\n" <<
                "    selfControllerEndpoint[" << selfControllerEndpoint << "]\n" <<
                "    selfHeartbeatDelay[" << selfHeartbeatDelay.count() << "]\n" <<
                "    brokerHeartbeatDelay[" << brokerHeartbeatDelay.count() << "]\n" <<
                "    brokerPulseDelay[" << brokerPulseDelay.count() << "]" );

            ret = activateControllerSockets();
            checkRet( ret, 0, "Worker.start: failed to activate controller sockets" );

            ret = setupControllerCallbacks();
            checkRet( ret, 0, "Worker.start: failed to su controller cbs" );

            // std::ref is required because worker
            // is not copyable but thread only allows pass by value
            reactorThread = std::unique_ptr<std::thread>(
                new std::thread(
                    &Worker::StartAsPrimary,
                    std::ref( *this ) ) );

            if ( !( reactorThread ) )
            {
                ret = teardownControllerCallbacks();
                checkRet( ret, 0, "Worker.start: failed to td controller cbs" );

                ret = deactivateControllerSockets();
                checkRet( ret, 0,
                    "Worker.start: failed to deactivate controller sockets" );

                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "Worker.start: failed to start reactor loop in secondary thread" );
            }

            clusterLog( "Worker.start: end" );
        }

        void Worker::StartAsPrimary()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() );
            clusterLog( "Worker.startAsPrimary: starting reactor loop " );

#ifdef STATS
            dumpStatsHeaders();
#endif

            do
            {
                int runningRet = 0;

                setExitState( WorkerExitState::None );

                updateCurrentTime();

                ret = activateMessagingSockets();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: "
                    "failed to activate messaging sockets" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                ret = setupMessagingCallbacks();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: "
                    "failed to setup messaging callbacks" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                requestProcessor.Start();

                ret = setupProcessorCallbacks();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: "
                    "failed to setup handler callbacks" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                updateCurrentTime();

                ret = sendBrokerReadyMessage();
                checkRet( ret, 0, "Worker.startAsPrimary: failed to send ready" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                if ( runningRet == 0 )
                {
                    ret = zloop_start( reactor );
                    checkRet( ret, 0, "Worker.startAsPrimary: failed to start loop" );

                    updateCurrentTime();

                    clusterLog( "Worker.startAsPrimary: "
                        "exiting reactor loop[" << ret << "]" );
                }
                else
                {
                    clusterLog( "Worker.startAsPrimary: errors encountered, not starting" );

                    setExitState( WorkerExitState::StartFailed );

                    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
                }

                ret = teardownProcessorCallbacks();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: failed to td handler cbs" );

                requestProcessor.Stop();

                ret = teardownMessagingCallbacks();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: failed to td messaging cbs" );

                ret = deactivateMessagingSockets();
                checkRet( ret, 0,
                    "Worker.startAsPrimary: "
                    "failed to deactivate messaging sockets" );

                if ( exitState != WorkerExitState::BrokerDied &&
                    exitState != WorkerExitState::StartFailed )
                {
                    clusterLog( "Worker.startAsPrimary: not restarting" );
                    break;
                }
                else
                {
                    clusterLog( "Worker.startAsPrimary: reconnecting" );
                }
            } while ( true );

            clusterLog( "Worker.startAsPrimary: end" );
        }

        int Worker::sendBrokerHeartbeat()
        {
            int ret = 0;

            clusterLogSetAction( "Worker.sendBrokerHeartbeat" );
            clusterLog( "Worker.sendBrokerHeartbeat: begin" );

            if ( heartbeatNeededAsOf( currentTime ) )
            {
                ZMQMessage* heartbeatMsg = zmsg_new();

                ret = SendHeartbeat(
                    messagingSocket,
                    currentTime,
                    selfHeartbeatDelay,
                    static_cast<std::uint32_t>( ClusterMessage::WorkerHeartbeat ),
                    &heartbeatMsg,
                    *senderReceiver );
                if ( ret != 0 )
                {
                    zmsg_destroy( &heartbeatMsg );
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Error,
                        "Worker.sendBrokerHeartbeat: failed to send heartbeat" );
                    IncrementStat( stats, WorkerStat::BrokerHeartbeatsSentFailed );
                    ret = 0;
                }
                else
                {
                    IncrementStat( stats, WorkerStat::BrokerHeartbeatsSent );
                    clusterLog( "Worker.sendBrokerHeartbeat: sent" );
                    updateWorkerLastActive( currentTime );
                }
            }
            else
            {
                clusterLog( "Worker.sendBrokerHeartbeat: heartbeat not needed" );
            }

            clusterLog( "Worker.sendBrokerHeartbeat: end" );

            return ret;
        }

        int Worker::handleProcessorMessage( ZMQMessage** lpProcessorMessage )
        {
            int ret = 0;
            ZMQFrameHandler frameHandler;
            ZMQFrame* frame = nullptr;
            RequestType requestType;
            RequestId requestId;
            RequestStatus requestStatus;
            ZMQMessage* processorMessage = *lpProcessorMessage;
            LHCVersionFlags vfs;

            clusterLogSetAction( "Worker.handleProcessorMessage2" )
                clusterLog( "Worker.handleProcessorMessage2: begin" )

                // 0) skip the flags
                frame = zmsg_first( processorMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpProcessorMessage );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleProcessorMessage2: "
                    "handler message is invalid or does not have version and flags" );
                IncrementStat( stats, WorkerStat::MalformedProcessorResponse );
                return 1;
            }
            vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
            // TODO - vfs - clusterLogSetFlags( vfs );

            // 1) parse the request type
            frame = zmsg_next( processorMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpProcessorMessage );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleProcessorMessage2: "
                    "handler message is invalid or does not have a request type" );
                IncrementStat( stats, WorkerStat::MalformedProcessorResponse );
                return 1;
            }
            requestType = frameHandler.get_frame_value< RequestType >( frame );
            clusterLogSetTag( requestType );

            // 2) parse the request id
            frame = zmsg_next( processorMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpProcessorMessage );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleProcessorMessage2: "
                    "handler message is invalid or does not have a request id" );
                IncrementStat( stats, WorkerStat::MalformedProcessorResponse );
                return 1;
            }
            requestId = frameHandler.get_frame_value< RequestId >( frame );

            // 3) parse the status, make sure that it is there
            frame = zmsg_next( processorMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpProcessorMessage );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleProcessorMessage2: "
                    "handler message is invalid or does not have a request status" );
                IncrementStat( stats, WorkerStat::MalformedProcessorResponse );
                return 1;
            }
            requestStatus = frameHandler.get_frame_value< RequestStatus >( frame );

            // try
            // {
            {
                auto itRequestTypeContext = requestTypeContexts.find(
                    requestType );
                auto itRequestContext = requestContexts.find(
                    std::pair< RequestType, RequestId >(
                        requestType, requestId ) );
                ZMQFrame* env = nullptr;

                if ( itRequestTypeContext == requestTypeContexts.end() )
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "Worker.handleProcessorMessage2: "
                        "no context for request type[" << requestType << "]" );
                    IncrementStat( stats, WorkerStat::MissingRequestTypeContext );

                    if ( itRequestContext != requestContexts.end() )
                    {
                        requestContexts.erase( itRequestContext );
                    }
                    else
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Warning,
                            "Worker.handleProcessorMessage2: "
                            "no context for request [" << requestId << "]" );
                        IncrementStat( stats, WorkerStat::MissingRequestContext );
                    }

                    zmsg_destroy( lpProcessorMessage );
                }
                else if ( itRequestContext == requestContexts.end() )
                {
                    WorkerRequestTypeContext& requestTypeContext =
                        ( *itRequestTypeContext ).second;
                    auto itRequestIdInProgress =
                        requestTypeContext.GetInProgressRequests().find( requestId );

                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "Worker.handleProcessorMessage2: "
                        "no context for request [" << requestId << "]" );
                    IncrementStat( stats, WorkerStat::MissingRequestContext );

                    if ( itRequestIdInProgress ==
                        requestTypeContext.GetInProgressRequests().end() )
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Warning,
                            "Worker.handleProcessorMessage2: "
                            "request id not in progress[" << requestId << "]" );
                        IncrementStat( stats, WorkerStat::CompletedRequestsNotInProgress );
                    }
                    else
                    {
                        requestTypeContext.GetInProgressRequests().erase(
                            itRequestIdInProgress );
                    }

                    zmsg_destroy( lpProcessorMessage );
                }
                else
                {
                    WorkerRequestTypeContext& requestTypeContext =
                        ( *itRequestTypeContext ).second;
                    ClientRequestContext& requestContext =
                        ( *itRequestContext ).second;
                    auto itRequestIdInProgress =
                        requestTypeContext.GetInProgressRequests().find( requestId );

                    if ( itRequestIdInProgress ==
                        requestTypeContext.GetInProgressRequests().end() )
                    {
                        zmsg_destroy( lpProcessorMessage );
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Warning,
                            "Worker.handleProcessorMessage2: "
                            "request id not in progress[" << requestId << "]" );
                        IncrementStat( stats, WorkerStat::CompletedRequestsNotInProgress );
                    }
                    else
                    {
                        auto itEnv = requestContext.GetEnvelope().rbegin();
                        // Start to modify the handler message so that
                        // it can be forwarded to the broker
                        // as an optimization, add the broker message first
                        // if( ! requestContext.GetVersionMessageFlags().hasAny(
                        //     MessageFlag::Asynchronous ) )
                        // {
                        ret = frameHandler.prepend_message_value(
                            processorMessage, ClusterMessage::BrokerResponse );
                        checkRet( ret, 0, "failed to prepend my least favorite optimization" );
                        // }
                        // Delimit the envelope with a null delimiter
                        zmsg_pushmem( processorMessage, NULL, 0 );
                        while ( itEnv != requestContext.GetEnvelope().rend() )
                        {
                            env = *itEnv;
                            ++itEnv;
                            itEnv = std::reverse_iterator< ClientEnvelope::iterator >(
                                requestContext.GetEnvelope().erase( itEnv.base() ) );
                            zmsg_prepend( processorMessage, &env );
                        }

                        // add the header to indicate to the broker the
                        // type of message this is
                        ret = frameHandler.prepend_message_value(
                            processorMessage,
                            ClusterMessage::WorkerFinished );
                        if ( requestContext.GetVersionMessageFlags().hasAny(
                            MessageFlag::Asynchronous ) )
                        {
                            checkRet( ret, 0, "failed to prepend finished async" );
                            IncrementStat( stats, WorkerStat::AsyncRequestsCompleted );
                        }
                        else
                        {
                            checkRet( ret, 0, "failed to prepend finished sync" );
                            IncrementStat( stats, WorkerStat::SyncRequestsCompleted );
                        }

                        //Add the null delimiter to the beginning of the message
                        zmsg_pushmem( processorMessage, NULL, 0 );

                        ret = senderReceiver->Send( lpProcessorMessage, messagingSocket );
                        if ( ret != 0 )
                        {
                            zmsg_destroy( lpProcessorMessage );
                            clusterLogWithSeverity(
                                LHClusterNS::SeverityLevel::Error,
                                "Worker.handleProcessorMessage2: "
                                "failed to send forwarded response" );
                            IncrementStat( stats,
                                WorkerStat::RequestsCompletedSentFailed );
                        }
                        else
                        {
                            IncrementStat( stats,
                                WorkerStat::RequestsCompletedSent );
                            updateWorkerLastActive( currentTime );
                        }
                        clusterLog( "Worker.handleProcessorMessage2: handler response forwarded" );

                        requestTypeContext.GetInProgressRequests().erase(
                            itRequestIdInProgress );

                        if ( requestStatus == RequestStatus::FatalError )
                        {
                            clusterLogWithSeverity(
                                LHClusterNS::SeverityLevel::Fatal,
                                "Worker.handleProcessorMessage2: "
                                "fatal error encountered while processing "
                                "request id[" << requestId << "], exiting" );
                            ret = -1;
                            setExitState( WorkerExitState::ProcessorFatalError );
                        }
                    }

                    requestContexts.erase( itRequestContext );
                }
            }
            // }
            // catch( const std::exception& ex )
            // {
            //     clusterLog( "Worker.handleProcessorMessage: exception caught while "
            //                 "handling handler message - " << ex.what() );
            // }
            // catch( ... )
            // {
            //     clusterLog( "Worker.handleProcessorMessage: unknown exception caught "
            //                 "while handling handler message" );
            //     zmsg_destroy( &processorMessage );
            // }

            clusterLog( "Worker.handleProcessorMessage2: stop" )

                return ret;
        }

        // BOOST_LOG_NAMED_SCOPE( scopeName );
        // BOOST_LOG_SCOPED_THREAD_TAG( LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTOR, boost::log::attributes::constant< std::string >( actor ) );
        // BOOST_LOG_SCOPED_THREAD_TAG( LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTION, boost::log::attributes::constant< std::string >( action ) );
        int Worker::handleProcessorMessage()
        {
            int ret = 0;
            ZMQMessage* processorMessage = nullptr;

            clusterLogSetAction( "Worker.handleProcessorMessage" );

            clusterLog( "Worker.handleProcessorMessage: begin" );

            updateCurrentTime();

            processorMessage = senderReceiver->Receive( requestProcessorMessagingSocket );
            if ( !( processorMessage ) )
            {
                IncrementStat( stats, WorkerStat::FailedProcessorReceives );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleProcessorMessage: "
                    "unable to retrieve message from handler socket" );
                return 0;
            }

            ret = handleProcessorMessage( &processorMessage );
            ret = ret < 0 ? ret : 0;

            clusterLog( "Worker.handleProcessorMessage: end" );

            return ret;
        }

        int Worker::handleBrokerMessage( ZMQMessage** lpDealerMessage )
        {
            int ret = 0;
            ZMQMessage* dealerMessage = *lpDealerMessage;
            ZMQFrameHandler frameHandler;
            ZMQFrame* frame = nullptr;
            ClusterMessage messageType;

            clusterLogSetAction( "Worker.handleBrokerMessage" )
                clusterLog( "Worker.handleBrokerMessage: begin" )

                //1) pop the null delimiter
                frame = zmsg_pop( dealerMessage );
            if ( !( frame && ( zframe_size( frame ) == 0 ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerMessage: broker message "
                    "does not have null delimiter or invalid format" );
                if ( frame )
                    zframe_destroy( &frame );
                zmsg_destroy( lpDealerMessage );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            zframe_destroy( &frame );

            //2) pop and parse the message type
            frame = zmsg_pop( dealerMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerMessage: broker message is invalid "
                    "or does not have a broker message type" );
                if ( frame )
                    zframe_destroy( &frame );
                zmsg_destroy( lpDealerMessage );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 2;
            }
            messageType = frameHandler.get_frame_value< ClusterMessage >( frame );
            zframe_destroy( &frame );

            switch ( messageType )
            {
            case ClusterMessage::BrokerNewRequest:
            {
                // double pointer because the called func will
                // send and destroy the message
                ret = handleBrokerNewRequest( lpDealerMessage );
                checkRet( ret, 0,
                    "Worker.handleBrokerMessage: "
                    "failed to handle new request" );
                break;
            }
            case ClusterMessage::BrokerHeartbeat:
            {
                ret = handleBrokerHeartbeat( dealerMessage );
                zmsg_destroy( lpDealerMessage );
                checkRet( ret, 0,
                    "Worker.handleBrokerMessage: "
                    "failed to handle heartbeat" );
                break;
            }
            case ClusterMessage::BrokerShuttingDown:
            {
                ret = -1;
                setExitState( WorkerExitState::GracefulStop );
                zmsg_destroy( lpDealerMessage );
                clusterLog( "Worker.handleBrokerMessage: received shutting "
                    "down signal from broker" );
                break;
            }
            default:
            {
                zmsg_destroy( lpDealerMessage );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Warning,
                    "Worker.handleBrokerMessage: unsupported broker message type" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                ret = 3;
                break;
            }
            }

            return ret;
        }

        int Worker::handleBrokerMessage()
        {
            int ret = 0;
            ZMQMessage* dealerMessage = nullptr;

            clusterLogSetAction( "Worker.handleBrokerMessage" );
            clusterLog( "Worker.handleBrokerMessage: begin" );

            updateCurrentTime();

            dealerMessage = senderReceiver->Receive( messagingSocket );
            if ( !( dealerMessage ) )
            {
                IncrementStat( stats, WorkerStat::FailedBrokerReceives );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerMessage: unable to "
                    "retrieve broker message" );
                return 0;
            }

            ret = handleBrokerMessage( &dealerMessage );
            ret = ret > 0 ? 0 : ret;    // 0 is fine, <0 implies exit

            updateBrokerLastActive( currentTime );  // something came in

            clusterLog( "Worker.handleBrokerMessage: end" );

            return ret;
        }

        int Worker::checkBrokerPulse()
        {
            int ret = 0;

            clusterLogSetAction( "Worker.checkBrokerPulse" );
            clusterLog( "Worker.checkBrokerPulse: begin" );

            if ( !( isBrokerAliveAsOf( currentTime ) ) )
            {
                ret = -1;
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Warning,
                    "Worker.checkBrokerPulse: broker died" );
                setExitState( WorkerExitState::BrokerDied );
                IncrementStat( stats, WorkerStat::BrokerDeaths );
            }

            clusterLog( "Worker.checkBrokerPulse: end" );
            return ret;
        }

        int Worker::handleBrokerNewRequest( ZMQMessage** lpNewRequestMsg )
        {
            int ret = 0;
            ZMQFrameHandler frameHandler;
            ZMQFrame* frame = nullptr;
            ClientRequestContext newRequestContext;
            ZMQMessage* newRequestMsg = *lpNewRequestMsg;
            ZMQMessage* receiptMsg = nullptr;
            ZMQFrame* receiptFrame = nullptr;
            bool needReceipt = false;

            clusterLogSetAction( "Worker.handleBrokerNewRequest" );
            clusterLog( "Worker.handleBrokerNewRequest: begin" );

            IncrementStat( stats, WorkerStat::RequestsReceived );

            clusterLog( "Worker.handleBrokerNewRequest: parsing client envelope" );
            // Step 1 - parse the client envelope to the null delimiter
            // initial guess at the size of envelope
            newRequestContext.GetEnvelope().reserve( 2 );
            frame = zmsg_pop( newRequestMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to parse envelope" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }

            newRequestContext.GetEnvelope().push_back( frame );
            while ( ( frame = zmsg_pop( newRequestMsg ) ) &&
                ( zframe_size( frame ) > 0 ) )
            {
                newRequestContext.GetEnvelope().push_back( frame );
            }
            if ( !frame )
            {
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to parse env, missing null delim" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            // destroy the popped null delimieter
            zframe_destroy( &frame );

            // Step 2 - parse the version and flags
            frame = zmsg_first( newRequestMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to parse version and flags" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            newRequestContext.SetVersionMessageFlags(
                frameHandler.get_frame_value< LHCVersionFlags >( frame ) );

            needReceipt =
                newRequestContext.GetVersionMessageFlags().hasAny( MessageFlag::ReceiptOnReceive );

            if ( needReceipt )
            {
                receiptMsg = zmsg_new();
                if ( !receiptMsg )
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Error,
                        "Worker.handleBrokerNewRequest: failed to alloc receipt msg" );
                    return -1;
                }

                for ( auto it = newRequestContext.GetEnvelope().begin();
                    it != newRequestContext.GetEnvelope().end();
                    ++it )
                {
                    receiptFrame = zframe_dup( *it );
                    zmsg_append( receiptMsg, &receiptFrame );
                }

                zmsg_addmem( receiptMsg, NULL, 0 );
                frameHandler.append_message_value< ClusterMessage >(
                    receiptMsg, ClusterMessage::BrokerReceivedReceipt );
                frameHandler.prepend_message_value< ClusterMessage >(
                    receiptMsg, ClusterMessage::WorkerReceipt );
                zmsg_pushmem( receiptMsg, NULL, 0 );
            }

            clusterLog( "Worker.handleBrokerNewRequest: parsing request type" );
            // Step 3 - parse the request type
            frame = zmsg_next( newRequestMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                if ( needReceipt )
                    zmsg_destroy( &receiptMsg );
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to parse request type" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            newRequestContext.SetRequestType( frameHandler.get_frame_value< RequestType >( frame ) );
            clusterLogSetTag( newRequestContext.GetRequestType() );
            auto requestTypeContextsIt = requestTypeContexts.find(
                newRequestContext.GetRequestType() );
            if ( requestTypeContextsIt == requestTypeContexts.end() )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: received unsupported request" );
                setExitState( WorkerExitState::ReceivedUnsupportedRequest );
                if ( needReceipt )
                    zmsg_destroy( &receiptMsg );
                zmsg_destroy( lpNewRequestMsg );
                return -1;
            }

            if ( needReceipt )
            {
                receiptFrame = zframe_dup( frame );
                zmsg_append( receiptMsg, &receiptFrame );
            }

            clusterLog( "Worker.handleBrokerNewRequest: parsing request id" );
            // Step 4 - parse the request id
            frame = zmsg_next( newRequestMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                if ( needReceipt )
                    zmsg_destroy( &receiptMsg );
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to parse request id" );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            newRequestContext.SetRequestId( frameHandler.get_frame_value< RequestId >( frame ) );
            if ( needReceipt )
            {
                receiptFrame = zframe_dup( frame );
                zmsg_append( receiptMsg, &receiptFrame );
            }

#ifdef STATS
            if ( newRequestContext.GetVersionMessageFlags().hasAny( MessageFlag::Asynchronous ) )
                IncrementStat( stats, WorkerStat::AsyncRequestsReceived );
            else
                IncrementStat( stats, WorkerStat::SyncRequestsReceived );
#endif


            clusterLog( "Worker.handleBrokerNewRequest: check redirect" );
            if ( requestRedirected( newRequestContext ) )
            {
                if ( needReceipt )
                {
                    clusterLog( "Worker.handleBrokerNewRequest: sending receipt "
                        "of redirected request to broker" );
                    //zmsg_print( receiptMsg );
                    ret = senderReceiver->Send(
                        &receiptMsg, messagingSocket );
                    if ( ret == 0 )
                    {
                        IncrementStat( stats, WorkerStat::ReceiptsSent );
                        clusterLog( "Worker.handleBrokerNewRequest: "
                            "receipt sent" );
                    }
                    else
                    {
                        IncrementStat( stats, WorkerStat::ReceiptsSentFailed );
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Error,
                            "Worker.handleBrokerNewRequest: "
                            "receipt failed to send" );
                    }
                }

                zmsg_destroy( lpNewRequestMsg );
                IncrementStat( stats, WorkerStat::RedirectsReceived );
                clusterLog( "Worker: redirecting request["
                    << newRequestContext.GetRequestType()
                    << "|"
                    << newRequestContext.GetRequestId()
                    << "]" );
                return 0;
            }

            clusterLog( "Worker.handleBrokerNewRequest: sending to handler" );
            // Step 5 - the rest of the message should be forwarded to the handler
            ret = senderReceiver->Send( lpNewRequestMsg, requestProcessorMessagingSocket );
            if ( ret != 0 )
            {
                if ( needReceipt )
                    zmsg_destroy( &receiptMsg );
                zmsg_destroy( lpNewRequestMsg );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerNewRequest: "
                    "failed to forward request to handler" );
                IncrementStat( stats, WorkerStat::FailedProcessorSends );
                // TODO - this is temporary until
                //        I can add resilience to the various actors
                //        which will allow unsent messages to be buffered
                //        and resends attempted periodically
                //        For now, it is best to just die rather
                //        than keep the broker thinking that the request is
                //        okay
                ret = -1;
                setExitState( WorkerExitState::ProcessorInaccessable );
            }
            else
            {
                clusterLog( "Worker.handleBrokerNewRequest: adding inprogress request" );
                // Step 6 - add the type/id to inprogredss and create/add the context
                requestTypeContextsIt->second.GetInProgressRequests().insert(
                    newRequestContext.GetRequestId() );
                requestContexts.insert(
                    std::pair<
                    std::pair< RequestType, RequestId >,
                    ClientRequestContext&& >(
                        std::pair< RequestType, RequestId>(
                            newRequestContext.GetRequestType(),
                            newRequestContext.GetRequestId() ),
                        std::move( newRequestContext ) ) );
                if ( needReceipt )
                {
                    clusterLog( "Worker.handleBrokerNewRequest: "
                        "sending receipt or new request to broker" );
                    ret = senderReceiver->Send(
                        &receiptMsg, messagingSocket );
                    if ( ret == 0 )
                    {
                        IncrementStat( stats, WorkerStat::ReceiptsSent );
                        clusterLog( "Worker.handleBrokerNewRequest: "
                            "receipt sent" );
                    }
                    else
                    {
                        IncrementStat( stats, WorkerStat::ReceiptsSentFailed );
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Error,
                            "Worker.handleBrokerNewRequest: "
                            "receipt failed to send" );
                        ret = 0;
                    }
                }
            }

            clusterLog( "Worker.handleBrokerNewRequest: end" );

            return ret;
        }

        bool Worker::heartbeatNeededAsOf( std::time_t timeAsOf )
        {
            bool ret = false;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
            struct tm workerLastActive_tm = { 0 };
#pragma GCC diagnostic pop
            std::time_t nextExpectedHeartbeatAt = 0;

            clusterLogSetAction( "Worker.heartbeatNeededAsOf" );
            clusterLog( "Worker.heartbeatNeededAsOf: begin" );

            localtime_r( &workerLastActive, &workerLastActive_tm );

            workerLastActive_tm.tm_sec += brokerHeartbeatDelay.count();
            nextExpectedHeartbeatAt = mktime( &workerLastActive_tm );

            // another heartbeat is not required at this time, because of
            // recent communication
            if ( nextExpectedHeartbeatAt <= timeAsOf )
                ret = true;

            clusterLog( "Worker.heartbeatNeededAsOf: end" );

            return ret;
        }

        void Worker::updateWorkerLastActive( std::time_t lastActive )
        {
            workerLastActive = lastActive;
        }

        void Worker::updateBrokerLastActive( std::time_t lastActive )
        {
            brokerLastActive = lastActive;
        }

        int Worker::sendBrokerReadyMessage()
        {
            int ret = 0;
            ZMQMessage* readyMsg = nullptr;
            ZMQFrameHandler frameHandler;
            const std::vector< RequestType >& supportedRequestTypes =
                requestProcessor.SupportedRequestTypes();
            unsigned int freeCapacity = capacity;
            LHCVersionFlags vfs;

            clusterLogSetAction( "Worker.sendBrokerReadyMessage" );
            clusterLog( "Worker.sendBrokerReadyMessage: begin" );

            readyMsg = zmsg_new();
            if ( readyMsg == nullptr )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.sendBrokerReadyMessage: failed to send" );
                IncrementStat( stats, WorkerStat::ReadiesSentFailed );
                return 0;
            }
            zmsg_addmem( readyMsg, NULL, 0 );

            // Add the header to indicate this is a "WorkerReady" message
            frameHandler.append_message_value( readyMsg, ClusterMessage::WorkerReady );

            frameHandler.append_message_value( readyMsg, vfs );

            for ( auto it1 = requestTypeContexts.begin();
                it1 != requestTypeContexts.end();
                ++it1 )
                freeCapacity -= ( *it1 ).second.GetInProgressRequests().size();

            // Add the currently free capacity
            // finished with request messages and not update the capacity
            // of the worker.
            // I should send the currently free capacity with the heartbeat, maybe
            frameHandler.append_message_value( readyMsg, freeCapacity );

            // Add the heartbeat delay
            frameHandler.append_message_value( readyMsg, selfHeartbeatDelay.count() );

            // TODO - support sending request ids of what is in progress
            for ( auto it = supportedRequestTypes.begin();
                it != supportedRequestTypes.end();
                ++it )
                frameHandler.append_message_value( readyMsg, *it );

            ret = senderReceiver->Send( &readyMsg, messagingSocket );
            if ( ret == 0 )
            {
                IncrementStat( stats, WorkerStat::ReadiesSent );
                updateWorkerLastActive( currentTime );
            }
            else
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.sendBrokerReadyMessage: failed to send" );
                IncrementStat( stats, WorkerStat::ReadiesSentFailed );
            }


            clusterLog( "Worker.sendBrokerReadyMessage: end" );
            return ret;
        }

        bool Worker::isBrokerAliveAsOf( std::time_t asOfTime )
        {
            bool ret = false;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
            struct tm brokerLastActive_tm = { 0 };
#pragma GCC diagnostic pop
            std::time_t deadAt = 0;

            localtime_r( &brokerLastActive, &brokerLastActive_tm );

            brokerLastActive_tm.tm_sec += brokerDeathDelay.count();
            deadAt = mktime( &brokerLastActive_tm );

            if ( deadAt >= asOfTime )
                ret = true;

            return ret;
        }

        void Worker::updateCurrentTime()
        {
            currentTime = std::time( NULL );
        }

        // int Worker::send_broker_inprogress( )
        // {
        //     int ret = 0;
        //     ZMQMessage* inprogress_msg = nullptr;
        //     ZMQFrameHandler frameHandler;
        //     unsigned int freeCapacity = capacity;

        //     updateCurrentTime();

        //     clusterLog( "Worker.send_broker_inprogress: begin" );

        //     inprogress_msg = zmsg_new();
        //     zmsg_addmem( inprogress_msg, NULL, 0 ); //every message must start with a null delimiter

        //     //Add the header to indicate this is a "WorkerReady" message
        //     //TODO - maybe put this set,save,assign,addmem nonsense into some sort of fucntion?
        //     frameHandler.append_message_value( inprogress_msg, ClusterMessage::WorkerInProgress );

        //     for( auto it1 = requestTypeContexts.begin(); it1 != requestTypeContexts.end(); ++it1 )
        //     {
        //         freeCapacity -= ( *it1 ).second.GetInProgressRequests().size();
        //     }

        //     //Add the capacity
        //     frameHandler.append_message_value( inprogress_msg, freeCapacity );

        //     for( auto it1 = requestTypeContexts.begin(); it1 != requestTypeContexts.end(); ++it1 )
        //     {
        //         std::pair< const RequestType, WorkerRequestTypeContext >& rt_pair = ( *it1 );

        //         //append the request type
        //         frameHandler.append_message_value< RequestType >( inprogress_msg, rt_pair.first );
        //         for(    auto it2 = rt_pair.second.GetInProgressRequests().begin();
        //                 it2 !=  rt_pair.second.GetInProgressRequests().end();
        //                 ++it2 )
        //         {
        //             frameHandler.append_message_value< RequestId >( inprogress_msg, *it2 );    //append the request id
        //         }

        //         zmsg_addmem( inprogress_msg, NULL, 0 );    //append the null delimiter
        //     }

        //     senderReceiver->Send( &inprogress_msg , dealerSocket );

        //     //TODO - check return values and take appropriate action

        //     updateWorkerLastActive( currentTime );

        //     clusterLog( "Worker.send_broker_inprogress: end" );

        //     return ret;

        //     //The inprogress request requires a new message to be sent to the broker
        //     //The message has the format
        //     //  InProgress | <free capacity> [ |RequestType [|RequestId ]* |NULL ]+
        //     //  free capacity must be dynamically calculated
        // }

        int Worker::handleBrokerHeartbeat( ZMQMessage* heartbeatMsg )
        {
            int ret = 0;
            ZMQFrame* frame = nullptr;
            int newHeartbeatSendDelay = 0;
            ZMQFrameHandler frameHandler;

            clusterLogSetAction( "Worker.handleBrokerHeartbeat" );
            clusterLog( "Worker.handleBrokerHeartbeat: begin" );

            IncrementStat( stats, WorkerStat::BrokerHeartbeatsReceived );

            frame = zmsg_pop( heartbeatMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerHeartbeat: "
                    "unable to update heartbeat info for broker "
                    "because of malformed heartbeat message - "
                    "missing \"sent at\" frame" );
                if ( frame )
                    zframe_destroy( &frame );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 1;
            }
            // unused
            // std::time_t sentAt = 0;
            // sentAt = frameHandler.get_frame_value< std::time_t >( frame );
            zframe_destroy( &frame );

            frame = zmsg_pop( heartbeatMsg );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.handleBrokerHeartbeat: "
                    "unable to update heartbeat info for broker"
                    " because of malformed heartbeat message - "
                    "missing \"heartbeat delay\" frame" );
                if ( frame )
                    zframe_destroy( &frame );
                IncrementStat( stats, WorkerStat::MalformedBrokerMessages );
                return 2;
            }
            newHeartbeatSendDelay = frameHandler.get_frame_value< int >( frame );
            zframe_destroy( &frame );
            brokerHeartbeatDelay = Delay( newHeartbeatSendDelay );

            clusterLog( "Worker.handleBrokerHeartbeat: end" );

            return ret;
        }

        void Worker::addNewInProgressRequest( RequestType type, RequestId id )
        {
            auto requestTypeContextsIt = requestTypeContexts.find( type );
            if ( requestTypeContextsIt != requestTypeContexts.end() )
            {
                requestTypeContextsIt->second.GetInProgressRequests().insert( id );
            }
            else
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "Worker.addNewInProgressRequest: failed to "
                    "find context for type" );
            }
        }

        bool Worker::requestRedirected( ClientRequestContext& requestContext )
        {
            auto it = requestContexts.find(
                std::pair< RequestType, RequestId >(
                    requestContext.GetRequestType(), requestContext.GetRequestId() ) );
            bool ret = false;

            if ( it != requestContexts.end() )
            {
                it->second.UpdateEnvelope( requestContext );
                it->second.SetVersionMessageFlags( requestContext.GetVersionMessageFlags() );
                ret = true;
            }

            return ret;
        }

#ifdef STATS
        int Worker::dumpStatsHeaders()
        {
            clusterLogSetAction( "Stats.Worker" )
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    MessageStats< WorkerStat >::GetDelimitedStatNames() )

                return 0;
        }

        int Worker::dumpStats()
        {
            clusterLogSetAction( "Stats.Worker" )
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    stats.GetDelimitedStatValues() )
                ClearStats( stats );
            return 0;
        }
#endif
    }
}
