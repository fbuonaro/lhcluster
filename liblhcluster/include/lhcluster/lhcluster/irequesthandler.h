#ifndef __LHCLUSTER_IREQUESTHANDLER_H__
#define __LHCLUSTER_IREQUESTHANDLER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/zmqaddin.h>

#include <utility>
#include <vector>

namespace LHClusterNS
{
    class IRequestHandler
    {
        public:
            // the return value of the process method, which is 
            // implemented by subclasses
            typedef std::pair< RequestStatus, ZMQMessage* > ProcessorResult;

            //CTORS, DTOR
            virtual ~IRequestHandler();

            // METHODS
            // supported request types, implemented by 
            // subclasses of RequestHandler
            // TODO - get rid of this
            virtual const std::vector< RequestType >&
            SupportedRequestTypes() const = 0;

            // NOTE - msg is owned by caller, can be modified, copied from,
            //        but not destroyed or persist after call to Process
            // NOTE - Implementation MUST allocate a new message using zmsg_new()
            //        even if empty
            virtual ProcessorResult Process(
                const LHCVersionFlags& vfs,
                RequestType requestType,
                RequestId requestId,
                ZMQMessage* msg ) = 0;
    };
}

#endif
