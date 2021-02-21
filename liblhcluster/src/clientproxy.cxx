#include <lhcluster_impl/clientproxy.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

#include <chrono>
#include <thread>

namespace LHClusterNS
{
namespace Impl
{
#ifdef STATS
    int dumpStatsClientProxyReactor(
        ZMQReactor* reactor,
        int timerReactorId,
        void* lpClientProxy )
    {
        try
        {
            ( ( ClientProxy* ) lpClientProxy )->updateCurrentTime();
            return ( ( ClientProxy* ) lpClientProxy )->dumpStats();
        }
        catch( const std::exception& ex )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.dumpStatsClientProxyReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.dumpStatsClientProxyReactor: uncaught exception[ "
                    << ex.what() << "]" );
            }
            catch( ... )
            {}
            return -1;
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.dumpStatsClientProxyReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy.dumpStatsClientProxyReactor: uncaught unknown exception" );
            }
            catch( ... )
            {}
            return -1;
        }
    }
#endif

    int clientClientMessagingReactor(
        ZMQReactor* reactor,
        ZMQSocket* clientMessagingSocket,
        void* lpClientProxy )
    {
        try
        {
            return
                ( ( ClientProxy* ) lpClientProxy )->handleClientMessage();
        }
        catch( const std::exception& ex )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientClientMessagingReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientClientMessagingReactor: "
                    "uncaught exception[ " << ex.what() << "]" );
            }
            catch( ... )
            {}
            return -1;
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientClientMessagingReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientClientMessagingReactor: "
                    "uncaught unknown exception" );
            }
            catch( ... )
            {}
            return -1;
        }
    }

    int clientBrokerMessagingReactor(
        ZMQReactor* reactor,
        ZMQSocket* brokerMessagingSocket,
        void* lpClientProxy )
    {
        try
        {
            return
                ( ( ClientProxy* ) lpClientProxy )->handleBrokerMessage();
        }
        catch( const std::exception& ex )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientBrokerMessagingReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientBrokerMessagingReactor: "
                    "uncaught exception[ " << ex.what() << "]" );
            }
            catch( ... )
            {}
            return -1;
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientBrokerMessagingReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientBrokerMessagingReactor: "
                    "uncaught unknown exception" );
            }
            catch( ... )
            {}
            return -1;
        }

    }

    int clientHeartbeatBrokerReactor(
        ZMQReactor* reactor,
        int timerReactorId,
        void* lpClientProxy )
    {
        try
        {
            return
                ( ( ClientProxy* ) lpClientProxy )->sendBrokerHeartbeat();
        }
        catch( const std::exception& ex )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientHeartbeatBrokerReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientHeartbeatBrokerReactor: "
                    "uncaught exception[ " << ex.what() << "]" );
            }
            catch( ... )
            {}
            return -1;
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientHeartbeatBrokerReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientHeartbeatBrokerReactor: "
                    "uncaught unknown exception" );
            }
            catch( ... )
            {}
            return -1;
        }

    }

    int clientPrimaryControllerReactor(
        ZMQReactor* reactor,
        SocketPair* listenerPairSocket,
        void* lpClientProxy )
    {
        ClientProxy* clientproxy = ( ClientProxy* ) lpClientProxy;
        ClientCommand command;
        ZMQMessage* signalMsg = nullptr;
        ZMQFrame* commandFrame = nullptr;
        int ret = 0;

        try
        {
            ZMQFrameHandler frameHandler;

            clusterLogSetAction( "ClientProxy.clientPrimaryControllerReactor" )
            clusterLog( "ClientProxy.clientPrimaryControllerReactor: "
                        "receiving command" );

            signalMsg = clientproxy->senderReceiver->Receive(
                listenerPairSocket );
            
            if( !( signalMsg ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy.clientPrimaryControllerReactor: "
                    "failed to receive command message" );
                return 0;
            }

            commandFrame = zmsg_pop ( signalMsg );
            if( !( commandFrame && zframe_size( commandFrame ) ) )
            {
                clusterLogWithSeverity( 
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy.clientPrimaryControllerReactor: command is malformed" );
                if( commandFrame )
                    zframe_destroy( &commandFrame );
                zmsg_destroy( &signalMsg );
                return 0;
            }

            command = frameHandler.get_frame_value< ClientCommand >(
                commandFrame );
            switch( command )
            {
                case ClientCommand::Stop:
                {
                    clusterLog(
                        "ClientProxy.clientPrimaryControllerReactor:"
                        "received stop command, exiting" );
                    clientproxy->setExitState(
                        ClientProxyExitState::GracefulStop );
                    ret = -1;
                    break;
                }
                default:
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "ClientProxy.clientPrimaryControllerReactor:"
                        "received unknown command" );
                    ret = 0;
                    break;
                }
            }

            zframe_destroy( &commandFrame );
            zmsg_destroy( &signalMsg );
            return ret;
        }
        catch( const std::exception& e )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientPrimaryControllerReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientPrimaryControllerReactor: "
                    "exception thrown[" << e.what() << "]" );
            }
            catch( ... )
            {}

            return -1;
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy.clientPrimaryControllerReactor" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "ClientProxy.clientPrimaryControllerReactor:"
                    "unknown exception thrown" );
            }
            catch( ... )
            {}

            return -1;
        }
    }

    int client_check_pulse_broker_reactor(
        ZMQReactor* reactor,
        int timerReactorId,
        void* lpClientProxy )
    {
        return ( ( ClientProxy* ) lpClientProxy )->checkBrokerPulse();
    }

    const char* ClientProxyConstructionFailed::what() const throw()
    {
        return "Failed to construct client proxy";
    }

    void ClientProxy::setExitState( ClientProxyExitState new_exitState )
    {
        exitState = new_exitState;
    }

    ClientProxy::ClientProxy(
        const Endpoint& _selfBrokerMessagingEndpoint, 
        const Endpoint& _brokerMessagingEndpoint, 
        const Endpoint& _selfControllerEndpoint,
        const Endpoint& _selfClientMessagingEndpoint,
        Delay _selfHeartbeatDelay,
        Delay _initialBrokerHeartbeatDelay,
        Delay _brokerPulseDelay,
        std::unique_ptr< IZMQSocketFactory > _socketFactory,
        std::unique_ptr< IZMQSocketSenderReceiver > 
            _senderReceiver )
    :   IClientProxy()
    ,   exitState( ClientProxyExitState::None )
    ,   selfClientMessagingEndpoint( _selfClientMessagingEndpoint )
        // ClientProxy's Broker facing address
    ,   selfBrokerMessagingEndpoint( _selfBrokerMessagingEndpoint ) 
        // Broker's client facing address
    ,   brokerMessagingEndpoint( _brokerMessagingEndpoint )
        // Address of local pair socket
    ,   selfControllerEndpoint( _selfControllerEndpoint )
        // Client router socket for receiving/sending
    ,   clientMessagingSocket( _socketFactory->Create( SocketType::Router ) )
        // Worker's dealer socket for sending/receiving
    ,   brokerMessagingSocket( _socketFactory->Create( SocketType::Dealer ) )
        // The Pair socket stop commands are received on
    ,   listenerSocket( _socketFactory->Create( SocketType::Pair ) )
        // The Pair socket stop commands are sent through
    ,   signalerSocket( _socketFactory->Create( SocketType::Pair ) )
        // Reactor for handling msgs,sending HBs, pulse
    ,   reactor( zloop_new() )
        // Numeric ID of reactor heartbeat callback 
    ,   heartbeatBrokerReactorId( -1 )                                   
        // Numeric ID of reactor check broker callback 
    ,   checkPulseBrokerReactorId( -1 )
        // Numeric ID of reactor dump stats callback
#ifdef STATS
    ,   dumpStatsReactorId( -1 )
#endif
        // Secondary thread where the reactor runs
    ,   reactorThread( nullptr )
        // Delay between sending heartbeats to broker
    ,   brokerHeartbeatDelay( _initialBrokerHeartbeatDelay )           
        // Delay between receiving heartbeats from broker
    ,   selfHeartbeatDelay( _selfHeartbeatDelay )
        // Delay between checking a broker's pulse
    ,   brokerPulseDelay( _brokerPulseDelay )
        // The inactivity length required to kill broker
    ,   brokerDeathDelay( 2*_selfHeartbeatDelay )                       
        // When the broker was last active
    ,   brokerLastActive( 0 )                                             
        // When this proxy last sent activity to broker
    ,   clientLastActive( 0 )                                             
#ifdef STATS
    ,   stats()
#endif
    ,   socketFactory( std::move( _socketFactory ) )
    ,   senderReceiver( std::move( _senderReceiver ) )
    {
        if( !( clientMessagingSocket &&
               brokerMessagingSocket &&
               reactor &&
               listenerSocket &&
               signalerSocket ) )
            throw ClientProxyConstructionFailed();
    }


    ClientProxy::~ClientProxy()
    {
        try
        {
            Stop();

            if( reactor )
            {
                zloop_destroy( &reactor );
            }

            if( clientMessagingSocket )
            {
                socketFactory->Destroy( &clientMessagingSocket );
            }

            if( brokerMessagingSocket )
            {
                socketFactory->Destroy( &brokerMessagingSocket );
            }
            
            if( signalerSocket )
            {
                socketFactory->Destroy( &signalerSocket );
            }

            if( listenerSocket )
            {
                socketFactory->Destroy( &listenerSocket );
            }
        }
        catch( ... )
        {
            try
            {
                clusterLogSetAction( "ClientProxy" )
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy: exception thrown in destructor" );
            }
            catch( ... )
            {}
        }
    }

    void ClientProxy::Stop()
    {
        int ret = 0;

        clusterLogSetActor( selfControllerEndpoint.path() )
        clusterLogSetAction( "ClientProxy.stop" );
        clusterLog( "ClientProxy.stop: begin" );

        if( reactorThread )
        {
            ZMQMessage* stopMsg = zmsg_new();
            ZMQFrameHandler frameHandler;
            int ret = 0;
            
            frameHandler.append_message_value< ClientCommand >(
                stopMsg,
                ClientCommand::Stop );
            ret = senderReceiver->Send( &stopMsg , signalerSocket );
            checkRet( ret, 0,
                      "ClientProxy.stop: failed to send stop signal" );
            if( reactorThread->joinable() )
            {
                clusterLog( "ClientProxy.stop: joining" );
                reactorThread->join();
            }
#ifdef DEBUG
            else
            {
                clusterLog( "ClientProxy.stop: not joining" );
            }
#endif

            reactorThread = nullptr;
        }

        ret = teardownControllerCallbacks();
        checkRet( ret, 0,
                  "ClientProxy.stop: "
                  "teardownControllerCallbacks failed["<<ret<<"]" );

        ret = deactivateControllerSockets();
        checkRet( ret, 0,
                  "ClientProxy.stop:"
                  "deactivateControllerSockets failed["<<ret<<"]" );

        BOOST_LOG_TRIVIAL( trace ) << "ClientProxy.stop: end";
    }

    int ClientProxy::activateControllerSockets()
    {
        int ret = 0;

        // Connect the listener and signaler
        ret = socketFactory->Bind(
            listenerSocket, selfControllerEndpoint.path() ) < 0;
        checkRet( ret, 0,
                  "ClientProxy.activateControllerSockets: failed to connect controller["
                  << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
        ret += socketFactory->Connect(
            signalerSocket, selfControllerEndpoint.path() ) < 0;
        checkRet( ret, 0,
                  "ClientProxy.activateControllerSockets: failed to connect controller["
                  << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

        return ret;
    }

    int ClientProxy::activateClientMessagingSockets()
    {
        int ret = 0;

        ret = socketFactory->Bind(
            clientMessagingSocket,
            selfClientMessagingEndpoint.path() ) < 0;
        checkRet( ret, 0,
                  "ClientProxy.activateClientMessagingSockets: failed to bind self["
                  << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

        return ret;
    }

    int ClientProxy::deactivateClientMessagingSockets()
    {
        int ret = 0;

        ret = socketFactory->UnBind(
            clientMessagingSocket,
            selfClientMessagingEndpoint.path() );

        return ret;
    }

    int ClientProxy::activateBrokerMessagingSockets()
    {
        int ret = 0;

        // Connect the dealer to the broker
        ret = socketFactory->Bind(
            brokerMessagingSocket,
            selfBrokerMessagingEndpoint.path() ) < 0;
        checkRet( ret, 0,
                  "ClientProxy.activateBrokerMessagingSockets: failed to bind self["
                  << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
        ret += socketFactory->Connect(
            brokerMessagingSocket,
            brokerMessagingEndpoint.path() ) < 0;
        checkRet( ret, 0,
                  "ClientProxy.activateBrokerMessagingSockets: failed to connect broker["
                  << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
        
        updateBrokerLastActive( currentTime );

        return ret;
    }

    int ClientProxy::teardownControllerCallbacks()
    {
        int ret = 0;

        zloop_reader_end( reactor, listenerSocket );

        return ret;
    }

    int ClientProxy::setupControllerCallbacks()
    {
        int ret = 0;

        ret = zloop_reader(
            reactor,
            listenerSocket,
            clientPrimaryControllerReactor,
            this );

        return ret;
    }

    int ClientProxy::setupClientMessagingCallbacks()
    {
        int ret = 0;

        ret = zloop_reader(
            reactor,
            clientMessagingSocket,
            clientClientMessagingReactor,
            this );

        return ret;
    }

    int ClientProxy::teardownClientMessagingCallbacks()
    {
        int ret = 0;

        zloop_reader_end( reactor, clientMessagingSocket );

        return ret;
    }

    int ClientProxy::setupBrokerMessagingCallbacks()
    {
        int ret = 0;
        std::chrono::milliseconds heartbeatSendDelay_ms =
            std::chrono::duration_cast< std::chrono::milliseconds >( 
                brokerHeartbeatDelay );
        std::chrono::milliseconds pulseCheckDelay_ms =
            std::chrono::duration_cast< std::chrono::milliseconds >( 
                brokerPulseDelay );
#ifdef STATS
        std::chrono::milliseconds dumpStatsDelay_ms =
            std::chrono::milliseconds( 5000 );
#endif

        // Setup reactor callback functions,
        // callback functions must be friends of the worker class
        // message handler
        ret = zloop_reader( reactor, 
                            brokerMessagingSocket, 
                            clientBrokerMessagingReactor, 
                            this );
        // check pulse of broker
        checkPulseBrokerReactorId = zloop_timer(
            reactor, 
            pulseCheckDelay_ms.count(), 
            0, 
            client_check_pulse_broker_reactor,        
            this );                        
        // send heartbeat to broker
        heartbeatBrokerReactorId = zloop_timer(
            reactor, 
            heartbeatSendDelay_ms.count(), 
            0, 
            clientHeartbeatBrokerReactor, 
            this );

#ifdef STATS
        dumpStatsReactorId =
            zloop_timer(
                reactor,
                dumpStatsDelay_ms.count(),
                0,
                dumpStatsClientProxyReactor,
                this );
#endif


        return ret;
    }

    int ClientProxy::teardownBrokerMessagingCallbacks()
    {
        int ret = 0;

        // Setup reactor callback functions,
        // callback functions must be friends of the worker class
        // message handler
        zloop_reader_end( reactor, brokerMessagingSocket );           
        // beartbeat broker
        zloop_timer_end( reactor, heartbeatBrokerReactorId );        
        // check pulse of broker
        zloop_timer_end( reactor, checkPulseBrokerReactorId );      
#ifdef STATS
        // dump stats
        zloop_timer_end( reactor, dumpStatsReactorId );
#endif

        return ret;
    }

    int ClientProxy::deactivateBrokerMessagingSockets()
    {
        int ret = 0;

        // disconnect dealer socket from broker and unbind dealer socket
        ret = socketFactory->DisConnect(
            brokerMessagingSocket, brokerMessagingEndpoint.path() );
        ret += socketFactory->UnBind(
            brokerMessagingSocket, selfBrokerMessagingEndpoint.path() );

        return ret;
    }

    int ClientProxy::deactivateControllerSockets()
    {
        int ret = 0;

        // disconnect and unbind pairs
        ret = socketFactory->DisConnect(
            signalerSocket, selfControllerEndpoint.path() );
        ret += socketFactory->UnBind(
            listenerSocket, selfControllerEndpoint.path() );

        ret += RefreshSocket(
            *socketFactory,
            &signalerSocket, SocketType::Pair );

        ret += RefreshSocket(
            *socketFactory,
            &listenerSocket, SocketType::Pair );

        return ret;
    }

    void ClientProxy::Start()
    {
        int ret = 0;

        clusterLogSetActor( selfControllerEndpoint.path() )
        clusterLogSetAction( "ClientProxy.start" );
        clusterLog( "ClientProxy.start: begin" );
        clusterLog( "ClientProxy.start: starting proxy with parameters\n" <<
                    "    selfBrokerMessagingEndpoint[" << selfBrokerMessagingEndpoint << "]\n"  <<
                    "    brokerMessagingEndpoint[" << brokerMessagingEndpoint << "]\n" <<
                    "    selfControllerEndpoint[" << selfControllerEndpoint << "]\n" <<
                    "    selfClientMessagingEndpoint[" << selfClientMessagingEndpoint << "]\n" <<
                    "    selfHeartbeatDelay[" << selfHeartbeatDelay.count() << "]\n" <<
                    "    brokerHeartbeatDelay[" << brokerHeartbeatDelay.count() << "]\n" <<
                    "    brokerPulseDelay[" << brokerPulseDelay.count() << "]" );
                    

        ret = activateControllerSockets();
        checkRet( ret, 0,
                  "ClientProxy.start: "
                  "failed to activate controller sockets" );

        ret = setupControllerCallbacks();
        checkRet( ret, 0,
                  "ClientProxy.start: "
                  "failed to su controller cbs" );

        // std::ref is required because worker is 
        // not copyable but thread only allows pass by value
        reactorThread = std::unique_ptr<std::thread>( 
            new std::thread(
                &ClientProxy::StartAsPrimary, std::ref( *this ) ) );

        if( !( reactorThread ) )
        {
            ret = teardownControllerCallbacks();
            checkRet( ret, 0,
                      "ClientProxy.start: failed to td controller cbs" );

            ret = deactivateControllerSockets();
            checkRet( ret, 0,
                      "ClientProxy.start: "
                      "failed to deactivate controller sockets" );
            clusterLog( "ClientProxy.start: "
                        "failed to start reactor loop in "
                        "secondary thread" );
        }

        clusterLog( "ClientProxy.start: end" );
    }

    void ClientProxy::StartAsPrimary()
    {
        int ret = 0;

        clusterLogSetActor( selfControllerEndpoint.path() )
        clusterLog( "ClientProxy.startAsPrimary: starting reactor loop" );

#ifdef STATS
        dumpStatsHeaders();
#endif

        do
        {
            int runningRet = 0;
            setExitState( ClientProxyExitState::None );    

            updateCurrentTime();

            ret = activateBrokerMessagingSockets();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to activate broker sockets" );
            runningRet += ( ( ret != 0 ) ? 1 : 0 );

            ret = setupBrokerMessagingCallbacks();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to setup broker callbacks" );
            runningRet += ( ( ret != 0 ) ? 1 : 0 );

            ret = activateClientMessagingSockets();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to activate client sockets" );
            runningRet += ( ( ret != 0 ) ? 1 : 0 );

            ret = setupClientMessagingCallbacks();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to setup client callbacks" );
            runningRet += ( ( ret != 0 ) ? 1 : 0 );

            for( auto it = requestsBuffer.UnderlyingBuffer().begin();
                 it != requestsBuffer.UnderlyingBuffer().end();
                 ++it )
            {
                ZMQMessage* copied_bufferedMsg = zmsg_dup( std::get< 2 >( it->second ) );
                ret = senderReceiver->Send(
                    &copied_bufferedMsg , brokerMessagingSocket );
                checkRet( ret, 0,
                          "ClientProxy.startAsPrimary: "
                          "failed to send buffered requests" );
                std::get< 1 >( it->second ) = std::get< 0 >( it->second ).flags;
            }

            if( runningRet == 0 )
            {
                ret = zloop_start( reactor );
                checkRet( ret, 0,
                          "ClientProxy.startAsPrimary: "
                          "failed to start loop" );

                clusterLog( "ClientProxy.startAsPrimary: "
                            "exiting reactor loop " << ret << "" );

            }
            else
            {
                clusterLog( "ClientProxy.startAsPrimary: errors encountered, not starting" );

                setExitState( ClientProxyExitState::StartFailed );    

                std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
            }

            updateCurrentTime();

            ret = teardownClientMessagingCallbacks();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to teardown client callbacks" );

            ret = deactivateClientMessagingSockets();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to deactivate client sockets" );

            ret = teardownBrokerMessagingCallbacks();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to teardown broker callbacks" );

            ret = deactivateBrokerMessagingSockets();
            checkRet( ret, 0,
                      "ClientProxy.startAsPrimary: "
                      "failed to deactivate broker sockets" );

            if( exitState != ClientProxyExitState::BrokerDied &&
                exitState != ClientProxyExitState::StartFailed )
            {
                clusterLog( "ClientProxy.startAsPrimary: "
                            "not restarting" );
                break;
            }
#ifdef DEBUG
            else
            {
                clusterLog( "ClientProxy.startAsPrimary: reconnecting" );
            }
#endif
        } while( true );
    }

    int ClientProxy::sendBrokerHeartbeat()
    {
        int ret = 0;

        clusterLogSetAction( "ClientProxy.sendBrokerHeartbeat" );
        clusterLog( "ClientProxy.sendBrokerHeartbeat: begin" );

        updateCurrentTime();

        if( heartbeatNeededAsOf( currentTime ) )
        {
            ZMQMessage* heartbeatMsg = zmsg_new();

            ret = SendHeartbeat(
                brokerMessagingSocket,
                currentTime,
                selfHeartbeatDelay,
                static_cast< std::uint32_t >( ClusterMessage::ClientHeartbeat ),
                &heartbeatMsg,
                *senderReceiver );
            if( ret == 0 )
            {
                IncrementStat( stats, ClientProxyStat::BrokerHeartbeatsSent );
                clusterLog( "ClientProxy.sendBrokerHeartbeat: sent" );
            }
            else
            {
                IncrementStat( stats, ClientProxyStat::BrokerHeartbeatsSentFailed );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy.sendBrokerHeartbeat: failed to send" );
            }


            updateClientLastActive( currentTime );
        }
#ifdef DEBUG
        else
        {
            clusterLog( "ClientProxy.sendBrokerHeartbeat: heartbeat not needed" );
        }
#endif

        clusterLog( "ClientProxy.sendBrokerHeartbeat: end" );
        
        return ret;
    }

    int ClientProxy::handleClientMessage( ZMQMessage** lpRouterMsg )
    {
        int ret = 0;
        ZMQFrameHandler frameHandler;
        ZMQFrame* frame = nullptr;

        clusterLogSetAction( "ClientProxy.handleClientMessage" )
        clusterLog( "ClientProxy.handleClientMessage: begin" )

        frame = zmsg_first( *lpRouterMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedClientRequests );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientMessage: "
                "router message does not have identity "
                "or invalid format" );
            zmsg_destroy( lpRouterMsg );
            return 0;
        }

        ret = handleClientRequest( frame, lpRouterMsg );
        ret = ret < 0 ? ret : 0;

        clusterLog( "ClientProxy.handleClientMessage: end" )

        return ret;
    }

    int ClientProxy::handleClientMessage()
    {
        int ret = 0;
        ZMQMessage* routerMsg = nullptr;

        clusterLogSetAction( "ClientProxy.handleClientMessage" );
        clusterLog( "ClientProxy.handleClientMessage: begin" );

        updateCurrentTime();
        routerMsg = senderReceiver->Receive( clientMessagingSocket );
        if( !( routerMsg ) )
        {
            IncrementStat( stats, ClientProxyStat::FailedClientReceives );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientMessage: unable to retrieve router message" );
            zmsg_destroy( &routerMsg );
            return 0;
        }

        ret = handleClientMessage( &routerMsg );
        ret = ret < 0 ? ret : 0;

        clusterLog( "ClientProxy.handleClientMessage: end" );

        return ret;
    }

    int ClientProxy::handleBrokerMessage( ZMQMessage** lpDealerMsg )
    {
        int ret = 0;
        ZMQMessage* dealerMsg = *lpDealerMsg;
        ZMQFrameHandler frameHandler;
        ZMQFrame* frame = nullptr;
        ClusterMessage messageType;

        clusterLogSetAction( "ClientProxy.handleBrokerMessage" )
        clusterLog( "ClientProxy.handleBrokerMessage: begin" )

        frame = zmsg_first( dealerMsg );
        while( frame && zframe_size( frame ) )
        {
            frame = zmsg_next( dealerMsg );
        }
        // frame should now be the null delimiter

        if( !( frame && ( zframe_size( frame ) == 0 ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerMessage: "
                "broker message does not have "
                "null delimiter or invalid format" );
            zmsg_destroy( lpDealerMsg );
            return 0;
        }

        //2) pop and parse the message type
        frame = zmsg_next( dealerMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerMessage: "
                "broker message is invalid or does not "
                "have a broker message type" );
            zmsg_destroy( lpDealerMsg );
            return 0;
        }
        messageType = frameHandler.get_frame_value< ClusterMessage >(
            frame );

        //remove and destroy the broker message type
        zmsg_remove( dealerMsg, frame );
        zframe_destroy( &frame );

        switch( messageType )
        {
            case ClusterMessage::BrokerSentReceipt:
            case ClusterMessage::BrokerReceivedReceipt:
            {
                ret = handleBrokerReceipt( messageType, lpDealerMsg );
                break;
            }
            case ClusterMessage::BrokerResponse:
            {
                ret = handleBrokerResponse( lpDealerMsg );    
                break;                                          
            }
            case ClusterMessage::BrokerHeartbeat:
            {
                ret = handleBrokerHeartbeat( lpDealerMsg );
                break;
            }
            default:
            {
                IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "ClientProxy.handleBrokerMessage: "
                    "unsupported broker message type["
                    << static_cast< std::uint32_t >( messageType )
                    << "]" );
                zmsg_destroy( lpDealerMsg );
                break;
            }
        }

        clusterLog( "ClientProxy.handleBrokerMessage: end" )

        return ret;
    }

    int ClientProxy::handleBrokerMessage()
    {
        int ret = 0;
        ZMQMessage* dealerMsg = nullptr;

        clusterLogSetAction( "ClientProxy.handleBrokerMessage" );
        clusterLog( "ClientProxy.handleBrokerMessage: begin" );

        updateCurrentTime();

        dealerMsg = senderReceiver->Receive( brokerMessagingSocket );
        if( !( dealerMsg ) )
        {
            IncrementStat( stats, ClientProxyStat::FailedBrokerReceives );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerMessage: "
                "unable to retrieve broker message" );
            return 0;
        }

        updateBrokerLastActive( currentTime );

        ret = handleBrokerMessage( &dealerMsg );
        ret = 0;

        clusterLog( "ClientProxy.handleBrokerMessage: end" );

        return ret;
    }

    int ClientProxy::checkBrokerPulse()
    {
        int ret = 0;

        updateCurrentTime();

        clusterLogSetAction( "ClientProxy.checkBrokerPulse" );
        clusterLog( "ClientProxy.checkBrokerPulse: begin" );

        if( !( isBrokerAliveAsOf( currentTime ) ) )
        {
            ret = -1;
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Warning,
                "ClientProxy.checkBrokerPulse: "
                "broker has died" );
            setExitState( ClientProxyExitState::BrokerDied );

            IncrementStat( stats, ClientProxyStat::BrokerDeaths );
        }

        clusterLog( "ClientProxy.checkBrokerPulse: end" );

        return ret;
    }

    bool ClientProxy::heartbeatNeededAsOf( std::time_t time_asof )
    {
        bool ret = false;
        struct tm clientProxyLastActive_tm = {0};
        std::time_t nextExpectedHeartbeatAt = 0;

        localtime_r( &clientLastActive, &clientProxyLastActive_tm );

        clientProxyLastActive_tm.tm_sec += brokerHeartbeatDelay.count();
        nextExpectedHeartbeatAt = mktime( &clientProxyLastActive_tm );

        // another heartbeat is not required at this time, because of
        // recent communication
        if( nextExpectedHeartbeatAt <= time_asof )    
        {                                               
            ret = true;
        }
        
        return ret;
    }

    void ClientProxy::updateClientLastActive( std::time_t lastActive )
    {
        clientLastActive = lastActive;
    }

    void ClientProxy::updateBrokerLastActive( std::time_t lastActive )
    {
        brokerLastActive = lastActive;
    }

    bool ClientProxy::isBrokerAliveAsOf( std::time_t asOfTime )
    {
        bool ret = false;
        struct tm brokerLastActive_tm = {0};
        std::time_t deadAt = 0;

        localtime_r( &brokerLastActive, &brokerLastActive_tm );

        brokerLastActive_tm.tm_sec += brokerDeathDelay.count();
        deadAt = mktime( &brokerLastActive_tm );

        if( deadAt >= asOfTime )
            ret = true;

        return ret;
    }

    void ClientProxy::updateCurrentTime()
    {
        currentTime = std::time( NULL );
    }

    int ClientProxy::handleBrokerReceipt(
        ClusterMessage receiptType, ZMQMessage** lpreceitMsg )
    {
        ZMQMessage* receiptMsg = *lpreceitMsg;
        int ret = 0;
        ZMQFrame* identity = nullptr;
        ZMQFrame* frame = nullptr;
        ZMQFrameHandler frameHandler;
        RequestType requestType;
        RequestId requestId;

        clusterLogSetAction( "ClientProxy.handleBrokerReceipt" );
        clusterLog( "ClientProxy.handleBrokerReceipt: begin" );

        // Parse the client's identity, only support 
        // single clients at the moment
        identity = zmsg_first( receiptMsg );
        if( !( identity && zframe_size( identity ) ) )
        {
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerReceipt: "
                "broker receipt missing client identity" );
            zmsg_destroy( lpreceitMsg );
            return 1;
        }

        // frame should be the null delimiter, skip it and 
        // move onto the type
        frame = zmsg_next( receiptMsg );
        if( !( frame ) || zframe_size( frame ) )
        {
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerReceipt: "
                "broker receipt missing null delimiter" );
            zmsg_destroy( lpreceitMsg );
            return 1;
        }

        frame = zmsg_next( receiptMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerReceipt: "
                "broker receipt missing request type" );
            zmsg_destroy( lpreceitMsg );
            return 1;
        }
        requestType = frameHandler.get_frame_value< RequestType >(
            frame );

        frame = zmsg_next( receiptMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerReceipt: "
                "broker receipt missing request id" );
            zmsg_destroy( lpreceitMsg );
            return 1;
        }
        requestId = frameHandler.get_frame_value< RequestId >( frame );

#ifdef DEBUG
        char* hexIdentity = zframe_strhex( identity );
        clusterLog( "ClientProxy: received receipt["
                    << requestType
                    << "|" << requestId
                    << "|" << hexIdentity << "]" );
        free( hexIdentity );
#endif
  
        zmsg_destroy( lpreceitMsg );

        requestsBuffer.EraseIfLastReceipt( receiptType, requestType, requestId, identity );

        IncrementStat( stats, ClientProxyStat::ReceiptsReceived );

        clusterLog( "ClientProxy.handleBrokerReceipt: end" );

        return ret;
    }

    //input lprequestMsg must begin with an identity followed by a null delimiter, and that the cursor is currently
    //positioned such that _next returns the null delimiter
    int ClientProxy::handleClientRequest(
        ZMQFrame* identity,
        ZMQMessage** lprequestMsg )
    {
        ZMQMessage* requestMsg = *lprequestMsg;
        int ret = 0;
        ZMQFrame* frame = nullptr;
        ZMQFrameHandler frameHandler;
        RequestType requestType;
        RequestId requestId;
        LHCVersionFlags vfs;

        clusterLogSetAction( "ClientProxy.handleClientRequest" );
        clusterLog( "ClientProxy.handleClientRequest: begin" );

        // 1) Get null delimiter
        frame = zmsg_next( requestMsg );
        if( !frame || zframe_size( frame ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedClientRequests );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientRequest: "
                "client request missing null delimiter" );
            zmsg_destroy( lprequestMsg );
            return 1;
        }

        // 2) skip ClusterMessage enum, only one type supported at the moment
        frame = zmsg_next( requestMsg ); 

        // 3) get the LHCVersionFlags
        frame = zmsg_next( requestMsg ); 
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedClientRequests );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientRequest: "
                "client request missing version and flags" );
            zmsg_destroy( lprequestMsg );
            return 1;
        }
        vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
        // TODO - vfs - clusterLogSetFlags( vfs.flags );

        // 4) get the request type
        frame = zmsg_next( requestMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedClientRequests );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientRequest: "
                "client request missing request type" );
            zmsg_destroy( lprequestMsg );
            return 1;
        }
        requestType = frameHandler.get_frame_value< RequestType >(
            frame );
        clusterLogSetTag( requestType );

        // 5) get the request id
        frame = zmsg_next( requestMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedClientRequests );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientRequest: "
                "client request missing request id" );
            zmsg_destroy( lprequestMsg );
            return 2;
        }
        requestId = frameHandler.get_frame_value< RequestId >( frame );

        if( vfs.hasAny( MessageFlag::ReceiptOnReceive | MessageFlag::ReceiptOnSend ) )
        {
            ZMQMessage* requestMsgCopy = zmsg_dup( requestMsg );
            requestsBuffer.Add(
                requestType,
                requestId,
                zmsg_first( requestMsgCopy ), // identity frame attached to copy
                vfs,
                requestMsgCopy );
        }

        ret = senderReceiver->Send( lprequestMsg, brokerMessagingSocket );
        if( ret != 0 )
        {
            IncrementStat( stats, ClientProxyStat::RequestsSentFailed );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleClientRequest: "
                "failed to forward client request" );
            zmsg_destroy( lprequestMsg );
            // TODO - send failure back to client
            //        or buffer in another container to be periodically flushed
        }
        else
        {
            IncrementStat( stats, ClientProxyStat::RequestsSent );
        }

        clusterLog( "ClientProxy.handleClientRequest: end" );

        return ret;
    }

    int ClientProxy::handleBrokerHeartbeat( ZMQMessage** lpheartbeatMsg )
    {
        ZMQMessage* heartbeatMsg = *lpheartbeatMsg;
        int ret = 0;
        ZMQFrame* frame = nullptr;
        std::time_t sentAt = 0;
        int newHeartbeatSendDelay = 0;
        ZMQFrameHandler frameHandler;

        clusterLogSetAction( "ClientProxy.handleBrokerHeartbeat" );
        clusterLog( "ClientProxy.handleBrokerHeartbeat: begin" );

        IncrementStat( stats, ClientProxyStat::BrokerHeartbeatsReceived );

        frame = zmsg_pop( heartbeatMsg );
        if( !( frame && ( zframe_size( frame ) == 0 ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerHeartbeat: "
                "unable to process heartbeat for "
                "broker - missing null delimiter" );
            if( frame )
                zframe_destroy( &frame );
            zmsg_destroy( lpheartbeatMsg );
            return 1;
        }
        zframe_destroy( &frame );

        frame = zmsg_pop( heartbeatMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerHeartbeat: "
                "unable to update heartbeat info for broker "
                "because of malformed heartbeat message - "
                "missing \"sent at\" frame" );
            if( frame )
                zframe_destroy( &frame );
            zmsg_destroy( lpheartbeatMsg );
            return 1;
        }
        sentAt = frameHandler.get_frame_value< std::time_t >( frame );
        zframe_destroy( &frame );

        frame = zmsg_pop( heartbeatMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerHeartbeat: "
                "unable to update heartbeat info for broker "
                "because of malformed heartbeat message - "
                "missing \"heartbeat delay\" frame" );
            if( frame )
                zframe_destroy( &frame );
            zmsg_destroy( lpheartbeatMsg );
            return 2;
        }
        newHeartbeatSendDelay = frameHandler.get_frame_value< int >(
            frame );
        zframe_destroy( &frame );

        brokerHeartbeatDelay = Delay( newHeartbeatSendDelay );

        zmsg_destroy( lpheartbeatMsg );

        clusterLog( "ClientProxy.handleBrokerHeartbeat: end" );

        return ret;
    }


    int ClientProxy::handleBrokerResponse( ZMQMessage** lpresponseMsg )
    {
        ZMQMessage* responseMsg = *lpresponseMsg;
        int ret = 0;
        ZMQFrame* frame = nullptr;
        ZMQFrame* identity = nullptr;
        ZMQFrameHandler frameHandler;
        RequestType requestType;
        RequestId requestId;
        char* hexIdentity = nullptr;
        LHCVersionFlags vfs;

        clusterLogSetAction( "ClientProxy.handleBrokerResponse" );
        clusterLog( "ClientProxy.handleBrokerResponse: begin" );

        // Parse the client's identity, only support 
        // single clients at the moment
        identity = zmsg_first( responseMsg );
        if( !( identity && zframe_size( identity ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "broker response missing client identity" );
            zmsg_destroy( lpresponseMsg );
            return 1;
        }

        // frame should be the null delimiter, skip it 
        // and move onto the type
        frame = zmsg_next( responseMsg );
        if( !( frame ) || zframe_size( frame ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "broker response missing null delimiter" );
            zmsg_destroy( lpresponseMsg );
            return 1;
        }

        // 1) skip version and flags
        frame = zmsg_next( responseMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "broker response missing version and flags" );
            zmsg_destroy( lpresponseMsg );
            return 1;
        }
        vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
        // TODO - vfs - clusterLogSetFlags( vfs )

        // 2) request type
        frame = zmsg_next( responseMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "broker response missing request type" );
            zmsg_destroy( lpresponseMsg );
            return 1;
        }
        requestType = frameHandler.get_frame_value< RequestType >(
            frame );
        clusterLogSetTag( requestType );

        // 3) request id
        frame = zmsg_next( responseMsg );
        if( !( frame && zframe_size( frame ) ) )
        {
            IncrementStat( stats, ClientProxyStat::MalformedBrokerMessages );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "broker response missing request id" );
            zmsg_destroy( lpresponseMsg );
            return 1;
        }
        requestId = frameHandler.get_frame_value< RequestId >( frame );

        hexIdentity = zframe_strhex( identity );


        clusterLog( "ClientProxy.handleBrokerResponse: received response["
                    << requestType
                    << "|" << requestId
                    << "|" << hexIdentity << "]" );
        free( hexIdentity );

        requestsBuffer.Erase( requestType, requestId, identity );

        IncrementStat( stats, ClientProxyStat::ResponsesReceived );

        ret = senderReceiver->Send( lpresponseMsg, clientMessagingSocket );
        if( ret != 0 )
        {
            IncrementStat( stats, ClientProxyStat::ResponsesSentFailed );
            clusterLogWithSeverity(
                LHClusterNS::SeverityLevel::Error,
                "ClientProxy.handleBrokerResponse: "
                "failed to forward broker response" );
            zmsg_destroy( lpresponseMsg );
        }
        else
        {
            IncrementStat( stats, ClientProxyStat::ResponsesSent );
        }

        clusterLog( "ClientProxy.handleBrokerResponse: end" );

        return ret;
    }

#ifdef STATS
    int ClientProxy::dumpStatsHeaders()
    {
        clusterLogSetAction( "Stats.ClientProxy" )
        clusterLogToChannel(
            LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
            MessageStats< ClientProxyStat >::GetDelimitedStatNames() )

        return 0;
    }

    int ClientProxy::dumpStats()
    {
        clusterLogSetAction( "Stats.ClientProxy" )
        clusterLogToChannel(
            LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
            stats.GetDelimitedStatValues() )
        ClearStats( stats );
        return 0;
    }
#endif
}
}
