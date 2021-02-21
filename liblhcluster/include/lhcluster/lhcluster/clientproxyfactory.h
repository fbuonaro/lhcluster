#ifndef __LHCLUSTER_CLIENTPROXYFACTORY_H__
#define __LHCLUSTER_CLIENTPROXYFACTORY_H__

#include <lhcluster/clientproxyparameters.h>
#include <lhcluster/iclientproxyfactory.h>

#include <lhcluster/clientproxy.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class ClientProxyFactory : public IClientProxyFactory
    {
        public:
            // CTORS, DTORS
            ~ClientProxyFactory()
            {
            }

            ClientProxyFactory()
            :   IClientProxyFactory()
            ,   iclientproxyfactory( IClientProxyFactory::GetDefaultFactory() )
            {
            }

            ClientProxyFactory( ClientProxyFactory&& other )
            :   IClientProxyFactory( std::move( other ) )
            ,   iclientproxyfactory( std::move( other.iclientproxyfactory ) )
            {
            }

            ClientProxyFactory& operator=( ClientProxyFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( ClientProxyFactory& a, ClientProxyFactory& b )
            {
                using std::swap;

                swap( a.iclientproxyfactory, b.iclientproxyfactory );
            }

            ClientProxy CreateConcrete( const ClientProxyParameters& p )
            {
                return ClientProxy( iclientproxyfactory->Create( p ) );
            }

            std::unique_ptr< IClientProxy > Create( const ClientProxyParameters& p )
            {
                return std::unique_ptr< IClientProxy >(
                    new ClientProxy( iclientproxyfactory->Create( p ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< IClientProxyFactory > iclientproxyfactory;
    };
}

#endif
