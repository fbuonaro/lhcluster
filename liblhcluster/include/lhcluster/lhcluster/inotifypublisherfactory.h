#ifndef __LHCLUSTER_INOTIFYPUBLISHERFACTORY_H__
#define __LHCLUSTER_INOTIFYPUBLISHERFACTORY_H__

#include <lhcluster/inotifypublisher.h>
#include <lhcluster/notifypublisherparameters.h>

#include <memory>

namespace LHClusterNS
{
    class INotifyPublisherFactory
    {
        public:
            INotifyPublisherFactory();
            virtual ~INotifyPublisherFactory();

            virtual std::unique_ptr< INotifyPublisher > Create(
                const NotifyPublisherParameters& p ) = 0;

            static std::unique_ptr< INotifyPublisherFactory > GetDefaultFactory();
    };
}

#endif
