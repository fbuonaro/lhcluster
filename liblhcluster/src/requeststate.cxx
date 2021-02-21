#include <lhcluster_impl/requeststate.h>

namespace LHClusterNS
{
namespace Impl
{
    RequestState::RequestState(
        RequestId _requestId,
        const LHCVersionFlags& _vfs,
        ZMQMessage** lpMsg )
    :   requestId( _requestId )
    ,   flag( RequestStateFlag::Pending )
    ,   vfs( _vfs )
    ,   msg( *lpMsg )
    ,   tryCount( 0 )
    ,   state{}
    {
        // its destruction is done by the 
        // BrokerRequestTypeContext which contains
        // so no destructor
        // a destructor would need to check whether pending/in progress
        // whereas the context just knows via pendingClientRequests
        // which tracks which states have a valid message
        *lpMsg = nullptr;
    }

    RequestState::RequestState(
        RequestId _requestId,
        const LHCVersionFlags& _vfs,
        WorkerInfo* wi )
    :   requestId( _requestId )
    ,   flag( RequestStateFlag::InProgress )
    ,   vfs( _vfs )
    ,   msg( nullptr )
    ,   tryCount( 1 )
    ,   state{}
    {
        state.inprogress.assignedWorker = wi;
    }

    RequestState::~RequestState()
    {
        if( msg )
            zmsg_destroy( &msg );
    }

    void RequestState::Assign( WorkerInfo* wi )
    {
        flag = RequestStateFlag::InProgress;
        state.inprogress.assignedWorker = wi;
        ++tryCount;
    }

    void RequestState::UpdateRequest( ZMQMessage** lpMsg )
    {
        zmsg_destroy( &msg );
        msg = *lpMsg; 
        *lpMsg = nullptr;
    }

    void RequestState::UpdateRequest( const LHCVersionFlags& _vfs )
    {
        vfs = _vfs;
    }

    void RequestState::UpdateRequest( ZMQMessage** lpMsg, const LHCVersionFlags& _vfs )
    {
        zmsg_destroy( &msg );
        msg = *lpMsg; 
        *lpMsg = nullptr;
        vfs = _vfs;
    }

    void RequestState::SetRequest( ZMQMessage** lpMsg, const LHCVersionFlags& _vfs )
    {
        // assert msg == nullptr
        msg = *lpMsg; 
        *lpMsg = nullptr;
        vfs = _vfs;
    }

    void RequestState::SetQueueCursor( std::list< RequestState* >::iterator listIt )
    {
        flag = RequestStateFlag::Pending;
        state.pending.listIt = listIt;
    }

}
}
