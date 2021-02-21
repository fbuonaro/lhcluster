#ifndef __LHCLUSTER_NOTIFYPUBLISHERFACTORY_H__
#define __LHCLUSTER_NOTIFYPUBLISHERFACTORY_H__

#include <lhcluster/inotifypublisherfactory.h>
#include <lhcluster/notifypublisherparameters.h>

#include <lhcluster/notifypublisher.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class NotifyPublisherFactory : public INotifyPublisherFactory
    {
        public:
            // CTORS, DTORS
            ~NotifyPublisherFactory()
            {
            }

            NotifyPublisherFactory()
            :   INotifyPublisherFactory()
            ,   inotifypublisherfactory( INotifyPublisherFactory::GetDefaultFactory() )
            {
            }

            NotifyPublisherFactory( NotifyPublisherFactory&& other )
            :   INotifyPublisherFactory( std::move( other ) )
            ,   inotifypublisherfactory( std::move( other.inotifypublisherfactory ) )
            {
            }

            NotifyPublisherFactory& operator=( NotifyPublisherFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifyPublisherFactory& a, NotifyPublisherFactory& b )
            {
                using std::swap;

                swap( a.inotifypublisherfactory, b.inotifypublisherfactory );
            }

            NotifyPublisher CreateConcrete( const NotifyPublisherParameters& p )
            {
                return NotifyPublisher( inotifypublisherfactory->Create( p ) );
            }

            std::unique_ptr< INotifyPublisher > Create( const NotifyPublisherParameters& p )
            {
                return std::unique_ptr< INotifyPublisher >(
                    new NotifyPublisher( inotifypublisherfactory->Create( p ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< INotifyPublisherFactory > inotifypublisherfactory;
    };
}

#endif
