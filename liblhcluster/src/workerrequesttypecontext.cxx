#include <lhcluster_impl/workerrequesttypecontext.h>

namespace LHClusterNS
{
namespace Impl
{
    WorkerRequestTypeContext::WorkerRequestTypeContext( RequestType _requestType )
    :   requestType( _requestType )
    {
    }

    WorkerRequestTypeContext::WorkerRequestTypeContext( WorkerRequestTypeContext&& wrtc )
    :   requestType( wrtc.requestType )
    ,   inProgressRequests( std::move( wrtc.inProgressRequests ) )
    {
        wrtc.requestType = 0;
    }

    void WorkerRequestTypeContext::AddInProgress( RequestId requestId )
    {
        inProgressRequests.insert( requestId );
    }

    void WorkerRequestTypeContext::RequestHasFinished( RequestId requestId )
    {
        inProgressRequests.erase( requestId );
    }
}
}
