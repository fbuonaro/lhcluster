#ifndef __LHCLUSTER_IMPL_WORKERREQUESTTYPECONTEXT_H__
#define __LHCLUSTER_IMPL_WORKERREQUESTTYPECONTEXT_H__

#include <lhcluster/cluster.h>

#include <unordered_set>

namespace LHClusterNS
{
namespace Impl
{
    // Contains information regarding the current state of support 
    // of a specific request type for a worker
    class WorkerRequestTypeContext
    {
        public:
            // CTORS, DTORS
            WorkerRequestTypeContext( const WorkerRequestTypeContext& ) = delete;
            WorkerRequestTypeContext& operator=( const WorkerRequestTypeContext& ) = delete;
            WorkerRequestTypeContext& operator=( WorkerRequestTypeContext&& ) = delete;

            WorkerRequestTypeContext( RequestType _requestType );
            WorkerRequestTypeContext( WorkerRequestTypeContext&& wrtc );

            // METHODS
            void AddInProgress( RequestId requestId );
            void RequestHasFinished( RequestId requestId );
            RequestType GetRequestType() const
            {
                return requestType;
            }

            const std::unordered_set< RequestId >&
            GetInProgressRequests() const
            {
                return inProgressRequests;
            }

            std::unordered_set< RequestId >&
            GetInProgressRequests()
            {
                return inProgressRequests;
            }

        private:
            // DATA MEMBERS
            RequestType requestType;
            std::unordered_set< RequestId > inProgressRequests;
    };

}
}

#endif
