#ifndef __LHCLUSTER_INOTIFYBROKER_H__
#define __LHCLUSTER_INOTIFYBROKER_H__

namespace LHClusterNS
{
    class INotifyBroker
    {
        public:
            // CTORS, DTORS
            virtual ~INotifyBroker();

            // METHODS
            virtual void Start() = 0;
            virtual void Stop() = 0;
    };
}

#endif
