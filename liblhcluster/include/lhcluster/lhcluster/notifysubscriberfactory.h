#ifndef __LHCLUSTER_NOTIFYSUBSCRIBERFACTORY_H__
#define __LHCLUSTER_NOTIFYSUBSCRIBERFACTORY_H__

#include <lhcluster/inotifysubscriberfactory.h>
#include <lhcluster/inotifyhandler.h>
#include <lhcluster/notifysubscriberparameters.h>

#include <lhcluster/notifysubscriber.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace LHClusterNS
{
    class NotifySubscriberFactory : public INotifySubscriberFactory
    {
        public:
            // CTORS, DTORS
            NotifySubscriberFactory()
            :   INotifySubscriberFactory()
            ,   inotifysubscriberfactory( INotifySubscriberFactory::GetDefaultFactory() )
            {
            }

            NotifySubscriberFactory( NotifySubscriberFactory&& other )
            :   INotifySubscriberFactory( std::move( other ) )
            ,   inotifysubscriberfactory( std::move( other.inotifysubscriberfactory ) )
            {
            }

            NotifySubscriberFactory& operator=( NotifySubscriberFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifySubscriberFactory& a, NotifySubscriberFactory& b )
            {
                using std::swap;

                swap( a.inotifysubscriberfactory, b.inotifysubscriberfactory );
            }

            NotifySubscriber CreateConcrete(
                const NotifySubscriberParameters& p,
                std::unique_ptr< INotifyHandler > nh )
            {
                return NotifySubscriber( inotifysubscriberfactory->Create( p, move( nh ) ) );
            }

            NotifySubscriber CreateConcrete(
                const NotifySubscriberParameters& p,
                const std::vector< NotificationType >& _subscriptions,
                std::unique_ptr< INotifyHandler > nh )
            {
                return NotifySubscriber(
                    inotifysubscriberfactory->Create( p, _subscriptions, move( nh ) ) );
            }

            std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                std::unique_ptr< INotifyHandler > nh )
            {
                return std::unique_ptr< INotifySubscriber >( new NotifySubscriber(
                    inotifysubscriberfactory->Create( p, move( nh ) ) ) );
            }

            std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                const std::vector< NotificationType >& _subscriptions,
                std::unique_ptr< INotifyHandler > nh )
            {
                return std::unique_ptr< INotifySubscriber >( new NotifySubscriber(
                    inotifysubscriberfactory->Create( p, _subscriptions, move( nh ) ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< INotifySubscriberFactory > inotifysubscriberfactory;
    };
}

#endif
