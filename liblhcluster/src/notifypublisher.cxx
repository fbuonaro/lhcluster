#include <lhcluster_impl/notifypublisher.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

namespace LHClusterNS
{
    namespace Impl
    {
        NotifyPublisher::NotifyPublisher(
            const Endpoint& _proxyEndpoint,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver )
            : INotifyPublisher()
            , selfEndpoint()
            , proxyEndpoint( _proxyEndpoint )
            , selfPublisherSocket( _socketFactory->Create( SocketType::Publisher ) )
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
        {
            int ret = 0;

            clusterLogSetAction( "NotifyPublisher.constructor" )

                if ( !( selfPublisherSocket ) )
                {
                    throw std::runtime_error( "Failed to create NotifyPublisher" );
                }

            ret = activatePublisherSocket();
            checkRet( ret, 0,
                "NotifyPublisher: failed to activate publisher sockets" );
        }

        NotifyPublisher::NotifyPublisher(
            const Endpoint& _selfEndpoint,
            bool forSelf,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver )
            : INotifyPublisher()
            , selfEndpoint( _selfEndpoint )
            , proxyEndpoint()
            , selfPublisherSocket( _socketFactory->Create( SocketType::Publisher ) )
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
        {
            (void)forSelf; // only used to differentiate this constructor
            int ret = 0;

            clusterLogSetAction( "NotifyPublisher.constructor" )

                if ( !( selfPublisherSocket ) )
                {
                    throw std::runtime_error( "Failed to create NotifyPublisher" );
                }

            ret = activatePublisherSocket();
            checkRet( ret, 0,
                "NotifyPublisher: failed to activate publisher sockets" );
        }

        NotifyPublisher::NotifyPublisher(
            const Endpoint& _selfEndpoint,
            const Endpoint& _proxyEndpoint,
            std::unique_ptr< IZMQSocketFactory > _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver )
            : INotifyPublisher()
            , selfEndpoint( _selfEndpoint )
            , proxyEndpoint( _proxyEndpoint )
            , selfPublisherSocket( _socketFactory->Create( SocketType::Publisher ) )
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
        {
            int ret = 0;

            clusterLogSetAction( "NotifyPublisher.constructor" )

                if ( !( selfPublisherSocket ) )
                {
                    throw std::runtime_error( "Failed to create NotifyPublisher" );
                }

            ret = activatePublisherSocket();
            checkRet( ret, 0,
                "NotifyPublisher: failed to activate publisher sockets" );
        }

        NotifyPublisher::~NotifyPublisher()
        {
            clusterLogSetAction( "NotifyPublisher.destructor" )
                try
            {
                if ( selfPublisherSocket )
                {
                    int ret = deactivatePublisherSocket();
                    if ( ret )
                    {
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "NotifyPublisher: failed to deactivate publisher socker, ret=" << ret );
                    }
                    socketFactory->Destroy( &selfPublisherSocket );
                }
            }
            catch ( ... )
            {
                try
                {
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Fatal,
                        "NotifyPublisher: exception thrown in destructor" );
                }
                catch ( ... )
                {
                }
            }
        }

        //TODO - REFACTOR
        /*
        int NotifyPublisher::publish( NotificationType notifyType, RequestId requestId, const JSONValue& json_value )
        {
            ZMQFrameHandler frame_handler;
            ZMQMessage* publishMsg = nullptr;
            int ret = 0;

            publishMsg = zmsg_new();
            if( !( publishMsg ) )
            {
                return 1;
            }

            frame_handler.append_message_value_as_str< NotificationType >( publishMsg, notifyType );  //subscribe to string, therefore must publish as str
            frame_handler.append_message_value< RequestId >( publishMsg, requestId );
            frame_handler.append_message_value< JSONValue >( publishMsg, json_value );

            ret = senderReceiver->Send( &publishMsg, selfPublisherSocket );

            if( publishMsg )
                zmsg_destroy( &publishMsg );

            return ret;
        }
        */

        int NotifyPublisher::Publish(
            const NotificationType& notifyType )
        {
            ZMQFrameHandler frame_handler;
            ZMQMessage* publishMsg = nullptr;
            int ret = 0;

            clusterLogSetAction( "NotifyPublisher.Publish0" )

                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Info,
                    "NotifyPublisher.Publish: publishing[" << notifyType << "]" );

            publishMsg = zmsg_new();
            if ( !( publishMsg ) )
            {
                return 1;
            }

            frame_handler.append_message_value_as_str< NotificationType >(
                publishMsg,
                notifyType );  //subscribe to string, therefore must publish as str

            ret = senderReceiver->Send( &publishMsg, selfPublisherSocket );
            if ( ret )
                zmsg_destroy( &publishMsg );

            return ret;
        }

        int NotifyPublisher::Publish(
            const NotificationType& notifyType,
            RequestId requestId )
        {
            ZMQFrameHandler frame_handler;
            ZMQMessage* publishMsg = nullptr;
            int ret = 0;

            clusterLogSetAction( "NotifyPublisher.Publish1" )

                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Info,
                    "NotifyPublisher.Publish: publishing[" << notifyType << "]" );

            publishMsg = zmsg_new();
            if ( !( publishMsg ) )
            {
                return 1;
            }

            frame_handler.append_message_value_as_str< NotificationType >(
                publishMsg,
                notifyType );  //subscribe to string, therefore must publish as str
            frame_handler.append_message_value< RequestId >(
                publishMsg,
                requestId );

            ret = senderReceiver->Send( &publishMsg, selfPublisherSocket );
            if ( ret )
                zmsg_destroy( &publishMsg );

            return ret;
        }

        int NotifyPublisher::Publish(
            const NotificationType& notifyType,
            ZMQMessage** lpZMQMessage )
        {
            ZMQFrameHandler frame_handler;

            clusterLogSetAction( "NotifyPublisher.Publish2" )

                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Info,
                    "NotifyPublisher.Publish: publishing[" << notifyType << "]" );

            frame_handler.prepend_message_value_as_str< NotificationType >(
                *lpZMQMessage,
                notifyType );  //subscribe to string, therefore must publish as str

            return senderReceiver->Send( lpZMQMessage, selfPublisherSocket );
        }

        int NotifyPublisher::Publish(
            const NotificationType& notifyType,
            RequestId requestId,
            ZMQMessage** lpZMQMessage )
        {
            ZMQFrameHandler frame_handler;
            ZMQMessage* publishMsg = *lpZMQMessage;

            clusterLogSetAction( "NotifyPublisher.Publish3" )

                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Info,
                    "NotifyPublisher.Publish: publishing[" << notifyType << "]" );

            frame_handler.prepend_message_value< RequestId >(
                publishMsg,
                requestId );
            frame_handler.prepend_message_value_as_str< NotificationType >(
                publishMsg,
                notifyType );  //subscribe to string, therefore must publish as str

            return senderReceiver->Send( lpZMQMessage, selfPublisherSocket );
        }

        int NotifyPublisher::activatePublisherSocket()
        {
            int ret = 0;
            clusterLogSetAction( "NotifyPublisher.activatePublisherSocket" );
            clusterLog( "NotifyPublisher.activatePublisherSocket: begin" );

            clusterLog( "NotifyPublisher.activatePublisherSocket: starting with parameters\n" <<
                "    selfEndpoint[" << selfEndpoint << "]\n" <<
                "    proxyEndpoint[" << proxyEndpoint << "]" );

            if ( selfEndpoint.type() != EndpointType::None )
            {
                ret += socketFactory->Bind( selfPublisherSocket, selfEndpoint.path() );
                checkRet( ret, 0,
                    "NotifyPublisher.activatePublisherSocket: failed to bind["
                    << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            }

            if ( proxyEndpoint.type() != EndpointType::None )
            {
                ret += socketFactory->Connect( selfPublisherSocket, proxyEndpoint.path() );
                checkRet( ret, 0,
                    "NotifyPublisher.activatePublisherSocket: failed to connect["
                    << ret << "|" << zmq_strerror( zmq_errno() ) << "]" );
            }

            clusterLog( "NotifyPublisher.activatePublisherSocket: end" );

            return ret;
        }

        int NotifyPublisher::deactivatePublisherSocket()
        {
            int ret = 0;

            if ( selfEndpoint.type() != EndpointType::None )
                ret += socketFactory->UnBind( selfPublisherSocket, selfEndpoint.path() );

            if ( proxyEndpoint.type() != EndpointType::None )
                ret += socketFactory->DisConnect( selfPublisherSocket, proxyEndpoint.path() );

            return ret;
        }
    }
}
