#ifndef __LHCLUSTER_NOTIFYSUBSCRIBER_H__
#define __LHCLUSTER_NOTIFYSUBSCRIBER_H__

#include <lhcluster/inotifysubscriber.h>

#include <algorithm>
#include <memory.h>

namespace LHClusterNS
{
    class NotifySubscriberFactory;

    class NotifySubscriber : public INotifySubscriber
    {
        friend NotifySubscriberFactory;

        public:
            // CTORS, DTORS
            ~NotifySubscriber()
            {
            }

            NotifySubscriber( NotifySubscriber&& other )
            :   INotifySubscriber( std::move( other ) )
            ,   inotifysubscriber( std::move( other.inotifysubscriber ) )
            {
            }

            NotifySubscriber& operator=( NotifySubscriber other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifySubscriber& a, NotifySubscriber& b )
            {
                using std::swap;

                swap( a.inotifysubscriber, b.inotifysubscriber );
            }

            void Start()
            {
                inotifysubscriber->Start();
            }

            void Stop()
            {
                inotifysubscriber->Stop();
            }

            void StartAsPrimary()
            {
                inotifysubscriber->StartAsPrimary();
            }

            int Subscribe( const std::vector< NotificationType >& subscriptions,
                           DelayMS timeout )
            {
                inotifysubscriber->Subscribe( subscriptions, timeout );
            }

            int UnSubscribe( const std::vector< NotificationType >& subscriptions,
                           DelayMS timeout )
            {
                inotifysubscriber->UnSubscribe( subscriptions, timeout );
            }

        private:
            // CTORS
            NotifySubscriber( std::unique_ptr< INotifySubscriber > _inotifysubscriber )
            :   INotifySubscriber()
            ,   inotifysubscriber( move( _inotifysubscriber ) )
            {
            }

            //MEMBERS
            std::unique_ptr< INotifySubscriber > inotifysubscriber;
    };
}
    
#endif
