#ifndef __LHCLUSTER_CLIENTFACTORY_H__
#define __LHCLUSTER_CLIENTFACTORY_H__

#include <lhcluster/clientparameters.h>
#include <lhcluster/iclientfactory.h>

#include <lhcluster/client.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class ClientFactory : public IClientFactory
    {
        public:
            // CTORS, DTORS
            ~ClientFactory()
            {
            }

            ClientFactory()
            :   IClientFactory()
            ,   iclientfactory( IClientFactory::GetDefaultFactory() )
            {
            }

            ClientFactory( ClientFactory&& other )
            :   IClientFactory( std::move( other ) )
            ,   iclientfactory( std::move( other.iclientfactory ) )
            {
            }

            ClientFactory& operator=( ClientFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( ClientFactory& a, ClientFactory& b )
            {
                using std::swap;

                swap( a.iclientfactory, b.iclientfactory );
            }

            Client CreateConcrete( const ClientParameters& p )
            {
                return Client( iclientfactory->Create( p ) );
            }

            std::unique_ptr< IClient > Create( const ClientParameters& p )
            {
                return std::unique_ptr< IClient >( new Client( iclientfactory->Create( p ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< IClientFactory > iclientfactory;
    };
}

#endif
