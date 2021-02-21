#ifndef __LHCLUSTER_IMPL_UTILS_H__
#define __LHCLUSTER_IMPL_UTILS_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/debug.h>
#include <lhcluster_impl/hash_traits.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>

#include <chrono>

namespace LHClusterNS
{
namespace Impl
{
    //sends heartbeat to specified socket, the format of the message sent is:
    //  <lpheartbeat_msg> | null | json_header | send at | requested_heartbeat_delay
    int SendHeartbeat( ZMQSocket* socket, 
                       std::time_t sentAsOfTime, 
                       Delay heartbeatDelay,
                       std::uint32_t headerValue,
                       ZMQMessage** lpHeartbeatMsg,
                       IZMQSocketSenderReceiver& sender );

    int RefreshSocket(
        IZMQSocketFactory& factory,
        ZMQSocket** lpSocket,
        SocketType type );

    void ZMQMessageEnvCopy( ZMQMessage* to, ZMQMessage* from );
    void ZMQMessageEnvMove( ZMQMessage* to, ZMQMessage* from );
}
}

#endif
