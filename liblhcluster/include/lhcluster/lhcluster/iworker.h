#ifndef __LHCLUSTER_IWORKER_H__
#define __LHCLUSTER_IWORKER_H__

namespace LHClusterNS
{

    class IWorker
    {
        public:
            // CTORS, DTORS
            virtual ~IWorker();

            // METHODS
            // start polling incoming messages in a secondary thread
            virtual void Start() = 0;
            // start polling incoming messages in this thread
            virtual void StartAsPrimary() = 0;
            // stop polling incoming messages
            virtual void Stop() = 0;
    };
}


#endif
