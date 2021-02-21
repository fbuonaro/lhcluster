#ifndef __LHCLUSTER_ICLIENT_H__
#define __LHCLUSTER_ICLIENT_H__

#include <lhcluster/cluster.h>
#include <lhcluster/requestresponse.h>

#include <vector>

namespace LHClusterNS
{
    class IClient
    {
        public:
            // CTORS, DTORS
            virtual ~IClient();

            //METHODS
            virtual int Activate() = 0;
            virtual int Deactivate() = 0;
            //Full non-blocking
            virtual int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit ) = 0;
            //Block for timeout_s seconds
            virtual int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::seconds timeout_s ) = 0;
            //Block for timeout_ms seconds
            virtual int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::milliseconds timeout_ms ) = 0;
            virtual int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId ) = 0;
            virtual int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId ) = 0;
            virtual int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags ) = 0;
            virtual int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage ) = 0;
            virtual int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage  ) = 0;
            virtual int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags,
                ZMQMessage** lpZMQMessage  ) = 0;
    };
}

#endif
