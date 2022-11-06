#include <lhcluster_impl/notifysubscriber.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <chrono>
#include <string>
#include <thread>

#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

#define notificationTypeToStr( notifyTypeValue ) notifyTypeValue.c_str()

namespace LHClusterNS
{
    namespace Impl
    {
        using namespace google::protobuf;

#ifdef STATS
        int dumpStatsNotifySubscriberReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpNotifySubscriber )
        {
            (void)reactor;
            (void)timerReactorId;
            try
            {
                return ( (NotifySubscriber*)lpNotifySubscriber )->dumpStats();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "NotifySubscriber.dumpStatsNotifySubscriberReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.dumpStatsNotifySubscriberReactor: uncaught exception[ "
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
                    clusterLogSetAction( "NotifySubscriber.dumpStatsNotifySubscriberReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.dumpStatsNotifySubscriberReactor: "
                            "uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }
#endif

        int publisherNotificationReactor(
            ZMQReactor* reactor,
            SocketSubscriber* socket,
            void* lpSubscriber )
        {
            (void)reactor;
            (void)socket;
            try
            {
                int ret =
                    ( static_cast<NotifySubscriber*>(
                        lpSubscriber ) )->handlePublisherNotification();
                return ret < 0 ? ret : 0;
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "NotifySubscriber.publisherNotificationReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.publisherNotificationReactor: "
                            "uncaught exception[ " << ex.what() << "]" );
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
                    clusterLogSetAction( "NotifySubscriber.publisherNotificationReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.publisherNotificationReactor: "
                            "uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }

        }

        int subscriberPrimaryControllerReactor(
            ZMQReactor* reactor,
            SocketPair* socket,
            void* lpSubscriber )
        {
            (void)reactor;
            (void)socket;
            try
            {
                int ret =
                    ( static_cast<NotifySubscriber*>(
                        lpSubscriber ) )->handleControllerCommand();
                return ret;
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "NotifySubscriber.subscriberPrimaryControllerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.subscriberPrimaryControllerReactor: "
                            "uncaught exception[ " << ex.what() << "]" );
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
                    clusterLogSetAction( "NotifySubscriber.subscriberPrimaryControllerReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifySubscriber.subscriberPrimaryControllerReactor: "
                            "uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }

        int publisherReconnectReactor(
            ZMQReactor* reactor,
            int timerReactorId,
            void* lpSubscriber )
        {
            (void)reactor;
            (void)timerReactorId;
            ( static_cast<NotifySubscriber*>(
                lpSubscriber ) )->setExitState(
                    NotifySubscriberExitState::Reconnect );
            return -1;
        }

        NotifySubscriber::NotifySubscriber(
            const Endpoint& _selfControllerEndpoint,
            const Endpoint& _publisherEndpoint,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver,
            std::unique_ptr< INotifyHandler > _handler )
            : INotifySubscriber()
            , selfControllerEndpoint( _selfControllerEndpoint )
            , publisherEndpoint( _publisherEndpoint )
            , subscriptions()
            , reactor( zloop_new() )
            , listenerSocket( _socketFactory->Create( SocketType::Pair ) )
            , signalerSocket( _socketFactory->Create( SocketType::Pair ) )
            , selfSubscriberSocket( _socketFactory->Create( SocketType::Subscriber ) )
            // TODO - pass as parameter
            , reconnectDelay( 60 * 5 )
            , publisherReconnectReactorId( -1 )
#ifdef STATS
            // Numeric ID of reactor dump stats callback
            , dumpStatsReactorId( -1 )
#endif
            , reactorThread( nullptr )
#ifdef STATS
            , stats()
#endif
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
            , handler( std::move( _handler ) )
            , signalerPoller( nullptr )
        {
            if ( !( signalerSocket ) )
                throw std::runtime_error(
                    "Failed to create NotifySubscriber" );

            signalerPoller = zpoller_new( signalerSocket, NULL );

            if ( !( reactor &&
                listenerSocket &&
                signalerPoller &&
                selfSubscriberSocket &&
                handler ) )
                throw std::runtime_error(
                    "Failed to create NotifySubscriber" );
        }

        NotifySubscriber::NotifySubscriber(
            const Endpoint& _selfControllerEndpoint,
            const Endpoint& _publisherEndpoint,
            const std::vector< NotificationType >& _subscriptions,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver,
            std::unique_ptr< INotifyHandler > _handler )
            : INotifySubscriber()
            , selfControllerEndpoint( _selfControllerEndpoint )
            , publisherEndpoint( _publisherEndpoint )
            , subscriptions( _subscriptions )
            , reactor( zloop_new() )
            , listenerSocket( _socketFactory->Create( SocketType::Pair ) )
            , signalerSocket( _socketFactory->Create( SocketType::Pair ) )
            , selfSubscriberSocket( _socketFactory->Create( SocketType::Subscriber ) )
            , reconnectDelay( 60 * 5 )
            , publisherReconnectReactorId( -1 )
            , reactorThread( nullptr )
#ifdef STATS
            , stats()
#endif
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
            , handler( std::move( _handler ) )
            , signalerPoller( nullptr )
        {
            if ( !( signalerSocket ) )
                throw std::runtime_error(
                    "Failed to create NotifySubscriber" );

            signalerPoller = zpoller_new( signalerSocket, NULL );

            if ( !( reactor &&
                listenerSocket &&
                signalerPoller &&
                selfSubscriberSocket &&
                handler ) )
                throw std::runtime_error(
                    "Failed to create NotifySubscriber" );

        }

        NotifySubscriber::~NotifySubscriber()
        {
            try
            {
                Stop();

                if ( reactor )
                {
                    zloop_destroy( &reactor );
                }

                if ( selfSubscriberSocket )
                {
                    socketFactory->Destroy( &selfSubscriberSocket );
                }

                if ( listenerSocket )
                {
                    socketFactory->Destroy( &listenerSocket );
                }

                if ( signalerSocket )
                {
                    socketFactory->Destroy( &signalerSocket );
                }
            }
            catch ( ... )
            {
                clusterLogSetAction( "NotifySubscriber.destructor" );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Fatal,
                    "NotifySubscriber.destructor: exception thrown" );
            }
        }

        void NotifySubscriber::Start()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "NotifySubscriber.start" );
            clusterLog( "NotifySubscriber.start: begin" );

            clusterLog( "NotifySubscriber.start: starting with parameters\n" <<
                "    selfControllerEndpoint[" << selfControllerEndpoint << "]\n" <<
                "    publisherEndpoint[" << publisherEndpoint << "]" );

            ret = activateControllerSockets();
            checkRet( ret, 0,
                "NotifySubscriber.start: "
                "failed to activate controller sockets" );

            ret = setupControllerCallbacks();
            checkRet( ret, 0,
                "NotifySubscriber.start: "
                "failed to setup controller cbs" );

            reactorThread = std::unique_ptr< std::thread >(
                new std::thread(
                    &NotifySubscriber::StartAsPrimary,
                    std::ref( *this ) ) );

            if ( !( reactorThread ) )
            {
                ret = teardownControllerCallbacks();
                checkRet( ret, 0,
                    "NotifySubscriber.start: "
                    "failed to td controller cbs" );

                ret = deactivateControllerSockets();
                checkRet( ret, 0,
                    "NotifySubscriber.start: "
                    "failed to deactivate controller sockets" );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.start: failed to start" );
            }
#ifdef DEBUG
            else
            {
                clusterLog( "NotifySubscriber.start: started" );
            }
#endif

            clusterLog( "NotifySubscriber.start: end" );
        }

        void NotifySubscriber::Stop()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLog( "NotifySubscriber.stop: begin" );

            if ( reactorThread )
            {
                ret = sendCommand( NotifySubscriberCommand::Stop,
                    DelayMS( 100 ) );
                checkRet( ret, 0,
                    "NotifySubscriber.stop: failed to signal stop" );
                if ( reactorThread->joinable() )
                {
                    clusterLog( "NotifySubscriber.stop: joining" );
                    reactorThread->join();
                }
#ifdef DEBUG
                else
                {
                    clusterLog( "NotifySubscriber.stop: not joining" );
                }
#endif
                reactorThread = nullptr;
            }
#ifdef DEBUG
            else
            {
                clusterLog( "NotifySubscriber.stop: nothing to stop" );
            }
#endif

            ret = teardownControllerCallbacks();
            checkRet( ret, 0,
                "NotifySubscriber.stop: failed to td controller cbs" );

            ret = deactivateControllerSockets();
            checkRet( ret, 0,
                "NotifySubscriber.stop: "
                "failed to deactivate controller sockets" );

            BOOST_LOG_TRIVIAL( trace ) << "NotifySubscriber.stop: stopped";
        }

        int NotifySubscriber::Subscribe( const std::vector< NotificationType >& subscriptions,
            DelayMS timeout )
        {
            return subOrUnsub( NotifySubscriberCommand::Subscribe,
                subscriptions,
                timeout );
        }

        int NotifySubscriber::UnSubscribe( const std::vector< NotificationType >& subscriptions,
            DelayMS timeout )
        {
            return subOrUnsub( NotifySubscriberCommand::Subscribe,
                subscriptions,
                timeout );
        }

        int NotifySubscriber::subOrUnsub( NotifySubscriberCommand command,
            const std::vector< NotificationType >& subscriptions,
            DelayMS timeout )
        {
            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "NotifySubscriber.subOrUnsub" );
            clusterLog( "NotifySubscriber.subOrUnsub: begin" );

            ZMQFrameHandler frame_handler;
            ZMQMessage* subMsg = nullptr;
            int ret = 0;

            subMsg = zmsg_new();
            if ( !( subMsg ) )
            {
                clusterLog( "NotifySubscriber.subOrUnsub: failed to alloc subMsg" );
                return 1;
            }

            zmsg_pushmem( subMsg, NULL, 0 );

            for ( auto it = subscriptions.crbegin(); it != subscriptions.crend(); ++it )
            {
                frame_handler.prepend_message_value_as_str< std::string >( subMsg, *it );
            }

            frame_handler.prepend_message_value< NotifySubscriberCommand >(
                subMsg,
                command );

            // Subscribe/UnSubscribe | Sub1 | .. | SubN | NULL

            ret = senderReceiver->Send( &subMsg, signalerSocket );
            if ( ret != 0 )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.subOrUnsub: failed to send command" );
                zmsg_destroy( &subMsg );
                IncrementStat( stats, NotifySubscriberStat::FailedSendSubsriptions );
                ret = 2;
            }
            else
            {
                if ( zpoller_wait( signalerPoller, timeout.count() ) )
                {
                    ZMQMessage* retMsg( senderReceiver->Receive( signalerSocket ) );
                    if ( retMsg )
                    {
                        // TODO - status result
                        zmsg_destroy( &retMsg );
                    }
                }
                else
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "NotifySubscriber.subOrUnsub: timed out waiting for response" );
                    ret = 3;
                }
            }

            clusterLog( "NotifySubscriber.subOrUnsub: end" );

            return ret;
        }

        int NotifySubscriber::sendCommand( NotifySubscriberCommand command,
            DelayMS timeout )
        {
            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "NotifySubscriber.sendCommand" );
            clusterLog( "NotifySubscriber.sendCommand: begin" );

            ZMQFrameHandler frame_handler;
            ZMQMessage* subMsg = nullptr;
            int ret = 0;

            subMsg = zmsg_new();
            if ( !( subMsg ) )
            {
                clusterLog( "NotifySubscriber.sendCommand: failed to alloc subMsg" );
                return 1;
            }

            frame_handler.prepend_message_value< NotifySubscriberCommand >(
                subMsg,
                command );

            ret = senderReceiver->Send( &subMsg, signalerSocket );
            if ( ret != 0 )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.sendCommand: failed to send command" );
                zmsg_destroy( &subMsg );
                ret = 2;
            }
            else
            {
                if ( zpoller_wait( signalerPoller, timeout.count() ) )
                {
                    ZMQMessage* retMsg( senderReceiver->Receive( signalerSocket ) );
                    if ( retMsg )
                    {
                        // TODO - status result
                        zmsg_destroy( &retMsg );
                    }
                }
                else
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Warning,
                        "NotifySubscriber.sendCommand: timed out waiting for response" );
                    ret = 3;
                }
            }

            clusterLog( "NotifySubscriber.sendCommand: end" );

            return ret;
        }

        void NotifySubscriber::StartAsPrimary()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLog( "NotifySubscriber.startAsPrimary: begin" );

#ifdef STATS
            dumpStatsHeaders();
#endif

            do
            {
                int runningRet = 0;

                setExitState( NotifySubscriberExitState::None );

                ret = activateSubscriberSockets();
                checkRet( ret, 0,
                    "NotifySubscriber.startAsPrimary: "
                    "failed to activate sub sockets" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                ret = setupSubscriberCallbacks();
                checkRet( ret, 0,
                    "NotifySubscriber.startAsPrimary: "
                    "failed to su sub cbs" );
                runningRet += ( ( ret != 0 ) ? 1 : 0 );

                if ( runningRet == 0 )
                {
                    ret = zloop_start( reactor );
                    checkRet( ret, 0,
                        "NotifySubscriber.startAsPrimary: "
                        "failed to start loop" );

                    clusterLog( "NotifySubscriber.startAsPrimary: "
                        "exiting reactor loop" );
                }
                else
                {
                    clusterLog( "NotifySubscriber.startAsPrimary: errors encountered, not starting" );

                    setExitState( NotifySubscriberExitState::StartFailed );

                    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
                }

                ret = teardownSubscriberCallbacks();
                checkRet( ret, 0,
                    "NotifySubscriber.startAsPrimary: "
                    "failed to td sub cbs" );

                ret = deactivateSubscriberSockets();
                checkRet( ret, 0,
                    "NotifySubscriber.startAsPrimary: "
                    "failed to deactivate sub sockets" );

                if ( exitState == NotifySubscriberExitState::GracefulStop )
                {
                    clusterLog( "NotifySubscriber.startAsPrimary: "
                        "not reconnecting" );
                    break;
                }

                clusterLog( "NotifySubscriber.startAsPrimary: reconnecting" );
            } while ( true );

            clusterLog( "NotifySubscriber.startAsPrimary: end" );
        }

        int NotifySubscriber::setupSubscriberCallbacks()
        {
            int ret = 0;
            std::chrono::milliseconds reconnectDelay_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    reconnectDelay );
#ifdef STATS
            std::chrono::milliseconds dumpStatsDelay_ms =
                std::chrono::milliseconds( 5000 );
#endif

            ret = zloop_reader( reactor,
                selfSubscriberSocket,
                publisherNotificationReactor,
                this );
            publisherReconnectReactorId =
                zloop_timer(
                    reactor,
                    reconnectDelay_ms.count(),
                    0,
                    publisherReconnectReactor,
                    this );

#ifdef STATS
            dumpStatsReactorId =
                zloop_timer(
                    reactor,
                    dumpStatsDelay_ms.count(),
                    0,
                    dumpStatsNotifySubscriberReactor,
                    this );
#endif


            return ret;
        }

        int NotifySubscriber::teardownSubscriberCallbacks()
        {
            int ret = 0;

            zloop_reader_end( reactor, selfSubscriberSocket );
            zloop_timer_end( reactor, publisherReconnectReactorId );
#ifdef STATS
            // dump stats
            zloop_timer_end( reactor, dumpStatsReactorId );
#endif

            return ret;
        }

        int NotifySubscriber::activateSubscriberSockets()
        {
            int ret = 0;
            ZMQFrame_uint32_t frameUInt32;
            std::string stringBuffer;

            ret = socketFactory->Connect(
                selfSubscriberSocket, publisherEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "NotifySubscriber.activateSubscriberSockets: failed to connect controller["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            if ( !( ret ) )
            {
                for ( auto it = subscriptions.begin();
                    it != subscriptions.end();
                    ++it )
                {
                    if ( it->size() > 0 )
                    {
                        socketFactory->Subscribe(
                            selfSubscriberSocket,
                            it->c_str() );
                    }
#ifdef DEBUG
                    else
                    {
                        clusterLog(
                            "NotifySubscriber.activateSubscriberSockets: "
                            "received empty subscription" );
                    }
#endif
                }
            }

            return ret;
        }

        int NotifySubscriber::deactivateSubscriberSockets()
        {
            int ret = 0;

            ret = socketFactory->DisConnect(
                selfSubscriberSocket, publisherEndpoint.path() ) < 0;

            return ret;
        }

        int NotifySubscriber::activateControllerSockets()
        {
            int ret = 0;

            ret = socketFactory->Bind(
                listenerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "NotifySubscriber.activateControllerSockets: failed to bind controller["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            ret += socketFactory->Connect(
                signalerSocket, selfControllerEndpoint.path() ) < 0;
            checkRet( ret, 0,
                "NotifySubscriber.activateControllerSockets: failed to connect controller["
                << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );

            return ret;
        }

        int NotifySubscriber::deactivateControllerSockets()
        {
            int ret = 0;

            ret = socketFactory->DisConnect(
                signalerSocket, selfControllerEndpoint.path() );

            ret = socketFactory->UnBind(
                listenerSocket, selfControllerEndpoint.path() );

            ret = RefreshSocket(
                *socketFactory,
                &signalerSocket, SocketType::Pair );

            ret = RefreshSocket(
                *socketFactory,
                &listenerSocket, SocketType::Pair );

            return ret;
        }

        int NotifySubscriber::teardownControllerCallbacks()
        {
            int ret = 0;

            zloop_reader_end( reactor, listenerSocket );

            return ret;
        }

        int NotifySubscriber::setupControllerCallbacks()
        {
            int ret = 0;

            ret = zloop_reader(
                reactor,
                listenerSocket,
                subscriberPrimaryControllerReactor,
                this );

            return ret;
        }

        int NotifySubscriber::handleControllerCommand()
        {
            ZMQMessage* controllerMessage = nullptr;
            int ret = 0;

            clusterLogSetAction( "NotifySubscriber.handleControllerCommand" );
            clusterLog( "NotifySubscriber.handleControllerCommand: begin" );

            controllerMessage = senderReceiver->Receive( listenerSocket );
            if ( !( controllerMessage ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.handleControllerCommand: unable to retrieve message" );
                IncrementStat( stats, NotifySubscriberStat::FailedControllerReceives );
                return 0;
            }

            ret = handleControllerCommand( &controllerMessage );
            ret = ret < 0 ? ret : 0;

            /*
            */

            clusterLog( "NotifySubscriber.handleControllerCommand: end" );

            return ret;
        }

        int NotifySubscriber::handleControllerCommand( ZMQMessage** lpControllerMessage )
        {
            int ret = 0;
            NotifySubscriberCommand command;
            ZMQFrame* frame = nullptr;
            ZMQFrameHandler frameHandler;
            ZMQMessage* controllerMessage = *lpControllerMessage;

            clusterLogSetAction( "NotifySubscriber.handleControllerCommand" );
            clusterLog( "NotifySubscriber.handleControllerCommand: begin" );

            frame = zmsg_first( controllerMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.handleControllerCommand: "
                    "message is invalid or does not specify a command" );
                zmsg_destroy( lpControllerMessage );
                IncrementStat( stats, NotifySubscriberStat::MalformedCommandMessages );
                return 0;
            }
            command = frameHandler.get_frame_value< NotifySubscriberCommand >( frame );

            switch ( command )
            {
            case NotifySubscriberCommand::Stop:
            {
                clusterLog(
                    "NotifySubscriber.handleControllerCommand: "
                    "received stop signal from primary thread" );
                setExitState(
                    NotifySubscriberExitState::GracefulStop );
                return -1;
            }
            case NotifySubscriberCommand::Subscribe:
            case NotifySubscriberCommand::UnSubscribe:
            {
                clusterLog(
                    "NotifySubscriber.handleControllerCommand: "
                    "received subscribe/unsubscribe command" );
                bool subscribedOne = false;


                while ( ( frame = zmsg_next( controllerMessage ) ) &&
                    zframe_size( frame ) )
                {
                    // TODO - must subscibe to the literal string
                    //        no decoding can be done
                    //        if this breaks then need to fix the publisher
                    //        upstream to not encode the notificationtype
                    NotificationType subscription =
                        frameHandler.get_frame_value_as_str< NotificationType >( frame );

                    if ( command == NotifySubscriberCommand::Subscribe )
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Info,
                            "NotifySubscriber.handleControllerCommand: "
                            << " subscribing["
                            << notificationTypeToStr( subscription ) << "]" )
                            socketFactory->Subscribe(
                                selfSubscriberSocket,
                                notificationTypeToStr( subscription ) );
                    }
                    else
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Info,
                            "NotifySubscriber.handleControllerCommand: "
                            << " unsubscribing["
                            << notificationTypeToStr( subscription ) << "]" )
                            socketFactory->UnSubscribe(
                                selfSubscriberSocket,
                                notificationTypeToStr( subscription ) );
                    }

                    subscribedOne = true;
                }

                if ( !subscribedOne )
                {
                    clusterLog(
                        "NotifySubscriber.handleControllerCommand: "
                        "received no subscriptions to subscribe to/unsubscribe from" );

                    ret = 2;
                }

                // TODO - status result
                ret = senderReceiver->Signal( listenerSocket, 0 );

                break;
            }
            default:
            {
                clusterLog(
                    "NotifySubscriber.handleControllerCommand: "
                    "received unknown command" );
                IncrementStat( stats, NotifySubscriberStat::UnknownControllerCommand );
                ret = 3;
                break;
            }
            };


            clusterLog( "NotifySubscriber.handleControllerCommand: end" );

            return ret;
        }

        int NotifySubscriber::handlePublisherNotification()
        {
            int ret = 0;
            ZMQMessage* publisherMessage = nullptr;
            ZMQFrameHandler frameHandler;
            ZMQFrame* frame = nullptr;
            NotificationType notificationType;
            RequestStatus status;

            clusterLogSetAction( "NotifySubscriber.handlePublisherNotification" )
                clusterLog( "NotifySubscriber.handlePublisherNotification: "
                    "handling publisher message" );

            publisherMessage = senderReceiver->Receive( selfSubscriberSocket );
            if ( !( publisherMessage ) )
            {
                IncrementStat( stats, NotifySubscriberStat::FailedPublisherReceives );
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.handlePublisherNotification: "
                    "unable to retrieve publisher message" );
                return 0;
            }
            else
            {
                IncrementStat( stats, NotifySubscriberStat::NotificationsReceived );
            }

            frame = zmsg_pop( publisherMessage );
            if ( !( frame && zframe_size( frame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "NotifySubscriber.handlePublisherNotification: "
                    "publisher notification missing "
                    "notification type" );
                if ( frame )
                    zframe_destroy( &frame );
                zmsg_destroy( &publisherMessage );
                return 1;
            }
            notificationType =
                frameHandler.get_frame_value_as_str< NotificationType >( frame );
            zframe_destroy( &frame );
            clusterLogSetTag( notificationType );

            status = handler->Process( notificationType, publisherMessage );
            if ( status == RequestStatus::InProgress )
            {
                IncrementStat( stats, NotifySubscriberStat::NotificationsProcessed );
                // stay subscribed
            }
            else if ( status == RequestStatus::Succeeded )
            {
                IncrementStat( stats, NotifySubscriberStat::NotificationsProcessed );
                socketFactory->UnSubscribe(
                    selfSubscriberSocket,
                    notificationTypeToStr( notificationType ) );
            }
            else
            {
                IncrementStat( stats, NotifySubscriberStat::NotificationsFailed );

                if ( status == RequestStatus::FatalError )
                    ret = -1; // trigger handler stop
            }

            zmsg_destroy( &publisherMessage );

            clusterLog( "NotifySubscriber.handlePublisherNotification: "
                "finished handling publisher message" );

            return ret;
        }

        void NotifySubscriber::setExitState(
            NotifySubscriberExitState newExitState )
        {
            exitState = newExitState;
        }

#ifdef STATS
        int NotifySubscriber::dumpStatsHeaders()
        {
            clusterLogSetAction( "Stats.NotifySubscriber" )
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    MessageStats< NotifySubscriberStat >::GetDelimitedStatNames() )

                return 0;
        }

        int NotifySubscriber::dumpStats()
        {
            clusterLogSetAction( "Stats.NotifySubscriber" )
                clusterLogToChannel(
                    LHCLUSTER_LOGGER_CHANNEL_NAME_STATS,
                    stats.GetDelimitedStatValues() )
                ClearStats( stats );
            return 0;
        }
#endif
    }
}
