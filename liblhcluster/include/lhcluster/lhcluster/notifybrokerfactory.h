#ifndef __LHCLUSTER_NOTIFYBROKERFACTORY_H__
#define __LHCLUSTER_NOTIFYBROKERFACTORY_H__

#include <lhcluster/inotifybrokerfactory.h>
#include <lhcluster/notifybrokerparameters.h>

#include <lhcluster/notifybroker.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class NotifyBrokerFactory : public INotifyBrokerFactory
    {
        public:
            // CTORS, DTORS
            ~NotifyBrokerFactory()
            {
            }

            NotifyBrokerFactory()
            :   INotifyBrokerFactory()
            ,   inotifybrokerfactory( INotifyBrokerFactory::GetDefaultFactory() )
            {
            }

            NotifyBrokerFactory( NotifyBrokerFactory&& other )
            :   INotifyBrokerFactory( std::move( other ) )
            ,   inotifybrokerfactory( std::move( other.inotifybrokerfactory ) )
            {
            }

            NotifyBrokerFactory& operator=( NotifyBrokerFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifyBrokerFactory& a, NotifyBrokerFactory& b )
            {
                using std::swap;

                swap( a.inotifybrokerfactory, b.inotifybrokerfactory );
            }

            NotifyBroker CreateConcrete( const NotifyBrokerParameters& p )
            {
                return NotifyBroker( inotifybrokerfactory->Create( p ) );
            }

            std::unique_ptr< INotifyBroker > Create( const NotifyBrokerParameters& p )
            {
                return std::unique_ptr< INotifyBroker >(
                    new NotifyBroker( inotifybrokerfactory->Create( p ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< INotifyBrokerFactory > inotifybrokerfactory;
    };
}

#endif
