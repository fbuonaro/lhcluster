#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

namespace LHClusterNS
{
namespace Impl
{
    int RefreshSocket(
        IZMQSocketFactory& factory,
        ZMQSocket** lpSocket,
        SocketType type )
    {
        int ret = 0;
        ZMQSocket* socket = nullptr;

        factory.Destroy( lpSocket );
        socket = factory.Create( type );
        *lpSocket = socket;

        if( !(socket ) )
            ret = 1;

        return ret;
    }

    int SendHeartbeat( ZMQSocket* socket, 
                       std::time_t sentAsOfTime, 
                       Delay heartbeatDelay,
                       std::uint32_t headerValue,
                       ZMQMessage** lpHeartbeatMsg,
                       IZMQSocketSenderReceiver& sender )
    {
        ZMQFrameHandler frameHandler;
        ZMQMessage* heartbeat_msg = *lpHeartbeatMsg;
        int ret = 0;

        //append a null frame
        zmsg_addmem( heartbeat_msg, NULL, 0 );

        //Add the message header to indicate that this is a Heartbeat (determined by the caller)
        ret = frameHandler.append_message_value( heartbeat_msg, headerValue );

        //Add the time that this message was sent at
        ret = frameHandler.append_message_value( heartbeat_msg, sentAsOfTime );

        //This is the delay that the receiver will use between sending heartbeats to this sender
        ret = frameHandler.append_message_value( heartbeat_msg, heartbeatDelay.count() );

        ret = sender.Send( lpHeartbeatMsg , socket );

        return ret;
    }

    void ZMQMessageEnvCopy( ZMQMessage* to, ZMQMessage* from )
    {
        ZMQFrame* frame = zmsg_first( from );

        while( frame && zframe_size( frame ) )
        {
            ZMQFrame* dupFrame = zframe_dup( frame );
            zmsg_append( to, &dupFrame );
            frame = zmsg_next( from );
        }
    }

    void ZMQMessageEnvMove( ZMQMessage* to, ZMQMessage* from )
    {
        ZMQFrame* frame = zmsg_pop( from );

        while( frame && zframe_size( frame ) )
        {
            zmsg_append( to, &frame );
            frame = zmsg_pop( from );
        }

        // null delim
        if( frame )
            zframe_destroy( &frame );
    }
}
}
