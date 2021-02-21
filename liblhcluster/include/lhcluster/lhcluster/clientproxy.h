#ifndef __LHCLUSTER_CLIENTPROXY_H__
#define __LHCLUSTER_CLIENTPROXY_H__

#include <lhcluster/iclientproxy.h>

#include <algorithm>
#include <memory.h>

namespace LHClusterNS
{
    class ClientProxyFactory;

    class ClientProxy : public IClientProxy
    {
        friend ClientProxyFactory;

        public:
            // CTORS, DTORS
            ~ClientProxy()
            {
            }

            ClientProxy( ClientProxy&& other )
            :   IClientProxy( std::move( other ) )
            ,   iclientproxy( std::move( other.iclientproxy ) )
            {
            }

            ClientProxy& operator=( ClientProxy other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( ClientProxy& a, ClientProxy& b )
            {
                using std::swap;

                swap( a.iclientproxy, b.iclientproxy );
            }

            // start polling incoming messages in a secondary thread
            void Start()
            {
                iclientproxy->Start();
            }

            // start polling incoming messages in this thread
            void StartAsPrimary()
            {
                iclientproxy->StartAsPrimary();
            }

            // stop polling incoming messages
            void Stop()
            {
                iclientproxy->Stop();
            }

        private:
            // CTORS
            ClientProxy( std::unique_ptr< IClientProxy > _iclientproxy )
            :   IClientProxy()
            ,   iclientproxy( std::move( _iclientproxy ) )
            {
            }

            // MEMBERS
            std::unique_ptr< IClientProxy > iclientproxy;
    };
}
#endif
