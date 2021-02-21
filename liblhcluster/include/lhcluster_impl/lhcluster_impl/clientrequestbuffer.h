#ifndef __LHCLUSTER_IMPL_CLIENTREQUESTBUFFER_H__
#define __LHCLUSTER_IMPL_CLIENTREQUESTBUFFER_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/zmqaddin.h>

#include <map>

namespace LHClusterNS
{
namespace Impl
{
    // Contains client requests which have been sent to the broker and 
    // are awaiting some receipt of work having started
    class ClientRequestBuffer
    {
        typedef std::tuple< RequestType, RequestId, ZMQFrame* >
            ClientRequestBufferKey;
        typedef std::tuple< LHCVersionFlags, MessageFlags, ZMQMessage* >
            ClientRequestBufferValue;
        typedef std::map< ClientRequestBufferKey, ClientRequestBufferValue >
            ClientRequestBufferMap;

        public:
            // CTORS, DTORS
            ClientRequestBuffer( const ClientRequestBuffer& ) = delete;
            ClientRequestBuffer& operator=( const ClientRequestBuffer& ) = delete;
            ClientRequestBuffer& operator=( ClientRequestBuffer&& ) = delete;

            ClientRequestBuffer();
            ClientRequestBuffer( ClientRequestBuffer&& wrtc );
            ~ClientRequestBuffer();

            // METHODS
            // Buffers the request in case it needs to be resent
            // Assume that identity is apart request_msg
            void Add(
                RequestType requestType,
                RequestId requestId,
                ZMQFrame* identity,
                const LHCVersionFlags& vfs,
                ZMQMessage* request_msg );

            void EraseIfLastReceipt(
                ClusterMessage messageType,
                RequestType requestType,
                RequestId requestId,
                ZMQFrame* identity );

            void Erase(
                RequestType requestType,
                RequestId requestId,
                ZMQFrame* identity );

            const ClientRequestBufferMap& UnderlyingBuffer() const
            {
                return bufferedRequests;
            }

            ClientRequestBufferMap& UnderlyingBuffer()
            {
                return bufferedRequests;
            }

        private:
            // DATA MEMBERS
            ClientRequestBufferMap bufferedRequests;
    };

}
}

#endif
