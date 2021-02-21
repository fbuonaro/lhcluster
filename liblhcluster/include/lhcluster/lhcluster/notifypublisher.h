#ifndef __LHCLUSTER_NOTIFYPUBLISHER_H__
#define __LHCLUSTER_NOTIFYPUBLISHER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/inotifypublisher.h>

#include <algorithm>
#include <memory.h>

namespace LHClusterNS 
{
    class NotifyPublisherFactory;

    class NotifyPublisher : public INotifyPublisher
    {
        friend NotifyPublisherFactory;

        public:
            // CTORS, DTORS
            ~NotifyPublisher()
            {
            }

            NotifyPublisher( NotifyPublisher&& other )
            :   INotifyPublisher( std::move( other ) )
            ,   inotifypublisher( std::move( other.inotifypublisher ) )
            {
            }

            NotifyPublisher& operator=( NotifyPublisher other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifyPublisher& a, NotifyPublisher& b )
            {
                using std::swap;

                swap( a.inotifypublisher, b.inotifypublisher );
            }

            int Publish( const NotificationType& notifyType )
            {
                return inotifypublisher->Publish( notifyType );
            }

            int Publish( const NotificationType& notifyType, RequestId requestId )
            {
                return inotifypublisher->Publish( notifyType, requestId );
            }

            int Publish( const NotificationType& notifyType, ZMQMessage** lpZMQMessage )
            {
                return inotifypublisher->Publish( notifyType, lpZMQMessage );
            }

            int Publish(
                const NotificationType& notifyType, RequestId requestId, ZMQMessage** lpZMQMessage )
            {
                return inotifypublisher->Publish( notifyType, requestId, lpZMQMessage );
            }

        private:
            // CTORS
            NotifyPublisher( std::unique_ptr< INotifyPublisher > _inotifypublisher )
            :   INotifyPublisher()
            ,   inotifypublisher( move( _inotifypublisher ) )
            {
            }

            // MEMBERS
            std::unique_ptr< INotifyPublisher > inotifypublisher;
    };
}

#endif
