#ifndef __LHCLUSTER_IMPL_IZMQSOCKETFACTORY_H__
#define __LHCLUSTER_IMPL_IZMQSOCKETFACTORY_H__

#include <lhcluster_impl/zmqaddin.h>

namespace LHClusterNS
{
namespace Impl
{
    class IZMQSocketFactory
    {
        public:
            virtual ~IZMQSocketFactory();

            virtual ZMQSocket* Create( SocketType socketType ) = 0;
            virtual void Destroy( ZMQSocket** zmqSocket ) = 0;
            virtual int Connect( ZMQSocket* zmqSocket,
                                 const char* endpoint ) = 0;
            virtual int DisConnect( ZMQSocket* zmqSocket,
                                    const char* endpoint ) = 0;
            virtual int Bind( ZMQSocket* zmqSocket,
                              const char* endpoint ) = 0;
            virtual int UnBind( ZMQSocket* zmqSocket,
                                const char* endpoint ) = 0;
            virtual void Subscribe( ZMQSocket* zmqSocket,
                                    const char* stringNotifyType ) = 0;
            virtual void UnSubscribe( ZMQSocket* zmqSocket,
                                      const char* stringNotifyType ) = 0;
    };
}
}

#endif
