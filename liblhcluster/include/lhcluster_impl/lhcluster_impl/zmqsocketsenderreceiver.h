#ifndef __LHCLUSTER_IMPL_ZMQSOCKETSENDERRECEIVER_H__
#define __LHCLUSTER_IMPL_ZMQSOCKETSENDERRECEIVER_H__

#include <lhcluster_impl/izmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class ZMQSocketSenderReceiver : public IZMQSocketSenderReceiver
    {
        public:
            // CTORS, DTORS
            ~ZMQSocketSenderReceiver() {}

            // METHODS
            inline ZMQMessage* Receive( ZMQSocket* zmqSocket )
            {
                return zmsg_recv( zmqSocket );
            }

            inline int Send( ZMQMessage** zmqMessage, ZMQSocket* zmqSocket )
            {
                return zmsg_send( zmqMessage, zmqSocket );
            }

            inline int Signal( ZMQSocket* zmqSocket, int signal )
            {
                return zsock_signal( zmqSocket, signal );
            }
    };
}
}

#endif
