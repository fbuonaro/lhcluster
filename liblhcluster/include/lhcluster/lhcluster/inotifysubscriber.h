#ifndef __LHCLUSTER_INOTIFYSUBSCRIBER_H__
#define __LHCLUSTER_INOTIFYSUBSCRIBER_H__

#include <vector>
#include <lhcluster/cluster.h>

namespace LHClusterNS
{
    class INotifySubscriber
    {
        public:
            // CTORS, DTORS
            virtual ~INotifySubscriber();

            // METHODS
            virtual void Start() = 0;
            virtual void Stop() = 0;
            virtual void StartAsPrimary() = 0;
            virtual int Subscribe( const std::vector< NotificationType >& subscriptions,
                           DelayMS timeout ) = 0;
            virtual int UnSubscribe( const std::vector< NotificationType >& subscriptions,
                           DelayMS timeout ) = 0;
    };
}
    
#endif
