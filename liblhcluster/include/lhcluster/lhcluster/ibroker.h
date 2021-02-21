#ifndef __LHCLUSTER_IBROKER_H__
#define __LHCLUSTER_IBROKER_H__

namespace LHClusterNS
{
    class IBroker
    {

        public:
            // CTORS, DTORS
            virtual ~IBroker();

            // METHODS
            // start polling incoming messages
            virtual void Start() = 0;
            // stop polling incoming messages
            virtual void Stop() = 0;
            // start polling in the current thread,
            // taking control of the thread until canceled
            virtual void StartAsPrimary() = 0;
    };
}

#endif
