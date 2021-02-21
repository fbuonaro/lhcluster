#ifndef __LHCLUSTER_ICLIENTPROXY_H__
#define __LHCLUSTER_ICLIENTPROXY_H__

namespace LHClusterNS
{

    class IClientProxy
    {
        public:
            // CTORS, DTORS
            virtual ~IClientProxy();

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
