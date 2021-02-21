#ifndef __LHCLUSTER_IMPL_IZMQSOCKETSENDERRECEIVER_H__
#define __LHCLUSTER_IMPL_IZMQSOCKETSENDERRECEIVER_H__

#include <lhcluster_impl/zmqaddin.h>

namespace LHClusterNS
{
namespace Impl
{
    class IZMQSocketSenderReceiver 
    {
        public:
            virtual ~IZMQSocketSenderReceiver();

            virtual ZMQMessage* Receive( ZMQSocket* zmqSocket ) = 0;
            virtual int Send( ZMQMessage** zmqMessage, ZMQSocket* zmqSocket ) = 0;
            virtual int Signal( ZMQSocket* zmqSocket, int signal ) = 0;
    };
}
}

#endif
