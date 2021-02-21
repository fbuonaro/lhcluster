#ifndef __LHCLUSTER_INOTIFYBROKERFACTORY_H__
#define __LHCLUSTER_INOTIFYBROKERFACTORY_H__

#include <lhcluster/inotifybroker.h>
#include <lhcluster/notifybrokerparameters.h>

#include <memory>

namespace LHClusterNS
{
    class INotifyBrokerFactory
    {
        public:
            INotifyBrokerFactory();
            virtual ~INotifyBrokerFactory();

            virtual std::unique_ptr< INotifyBroker > Create(
                const NotifyBrokerParameters& p ) = 0;

            static std::unique_ptr< INotifyBrokerFactory > GetDefaultFactory();
    };
}

#endif
