#ifndef __LHCLUSTER_IMPL_ZMQSOCKETFACTORY_H__
#define __LHCLUSTER_IMPL_ZMQSOCKETFACTORY_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <string>

namespace LHClusterNS
{
namespace Impl
{
    class ZMQSocketFactory : public IZMQSocketFactory
    {
        public:
            // CTORS, DTORS
            ~ZMQSocketFactory() {}

            // METHODS
            inline ZMQSocket* Create( SocketType socketType )
            {
                ZMQSocket* zmqSocket = zsock_new( socketType );
                if( zmqSocket )
                    zsock_set_sndtimeo( zmqSocket, 3 );
                return zmqSocket;
            }

            inline void Destroy( ZMQSocket** zmqSocket )
            {
                zsock_destroy( zmqSocket );
            }

            inline int Connect( ZMQSocket* zmqSocket,
                                const char* endpoint )
            {
                return zsock_connect( zmqSocket, endpoint );
            }

            inline int DisConnect( ZMQSocket* zmqSocket,
                                   const char* endpoint )
            {
                return zsock_disconnect( zmqSocket, endpoint );
            }

            inline int Bind( ZMQSocket* zmqSocket,
                             const char* endpoint )
            {
                return zsock_bind( zmqSocket, endpoint );
            }

            inline int UnBind( ZMQSocket* zmqSocket,
                               const char* endpoint )
            {
                return zsock_unbind( zmqSocket, endpoint );
            }

            inline void Subscribe( ZMQSocket* zmqSocket,
                                   const char* stringNType )
            {
                zsock_set_subscribe( zmqSocket, stringNType );
            }

            inline void UnSubscribe( ZMQSocket* zmqSocket,
                                     const char* stringNType )
            {
                zsock_set_unsubscribe( zmqSocket, stringNType );
            }

            // TODO - unsubscribe
    };
}
}

#endif
