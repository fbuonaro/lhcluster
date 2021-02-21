#ifndef __LHCLUSTER_BROKERFACTORY_H__
#define __LHCLUSTER_BROKERFACTORY_H__

#include <lhcluster/brokerparameters.h>
#include <lhcluster/ibrokerfactory.h>

#include <lhcluster/broker.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class BrokerFactory : public IBrokerFactory
    {
        public:
            // CTORS, DTORS
            ~BrokerFactory()
            {
            }

            BrokerFactory()
            :   IBrokerFactory()
            ,   ibrokerfactory( IBrokerFactory::GetDefaultFactory() )
            {
            }

            BrokerFactory( BrokerFactory&& other )
            :   IBrokerFactory( std::move( other ) )
            ,   ibrokerfactory( move( other.ibrokerfactory ) )
            {
            }

            BrokerFactory& operator=( BrokerFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( BrokerFactory& a, BrokerFactory& b )
            {
                using std::swap;

                swap( a.ibrokerfactory, b.ibrokerfactory );
            }

            Broker CreateConcrete( const BrokerParameters& p )
            {
                return Broker( ibrokerfactory->Create( p ) );
            }

            std::unique_ptr< IBroker > Create( const BrokerParameters& p )
            {
                return std::unique_ptr< IBroker >( new Broker( ibrokerfactory->Create( p ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< IBrokerFactory > ibrokerfactory;
    };
}

#endif
