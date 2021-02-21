#ifndef __LHCLUSTER_INOTIFYSUBSCRIBERFACTORY_H__
#define __LHCLUSTER_INOTIFYSUBSCRIBERFACTORY_H__

#include <lhcluster/inotifysubscriber.h>
#include <lhcluster/inotifyhandler.h>
#include <lhcluster/notifysubscriberparameters.h>

#include <memory>
#include <vector>

namespace LHClusterNS
{
    class INotifySubscriberFactory
    {
        public:
            INotifySubscriberFactory();
            virtual ~INotifySubscriberFactory();

            virtual std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                std::unique_ptr< INotifyHandler > nh ) = 0;

            virtual std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                const std::vector< NotificationType >& _subscriptions,
                std::unique_ptr< INotifyHandler > nh ) = 0;

            static std::unique_ptr< INotifySubscriberFactory > GetDefaultFactory();
    };
}

#endif
