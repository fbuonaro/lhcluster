#include <lhcluster_impl/brokerrequesttypecontext.h>

namespace LHClusterNS
{
namespace Impl
{
    BrokerRequestTypeContext::BrokerRequestTypeContext( RequestType _requestType )
    :   totalWorkers( 0 )
    ,   requestType( _requestType )
    ,   availableWorkers( )
    ,   pendingClientRequests( )
    ,   requests( )
    {
    }

    BrokerRequestTypeContext::BrokerRequestTypeContext(
        BrokerRequestTypeContext&& other )
    :   totalWorkers( other.totalWorkers )
    ,   requestType( other.requestType )
    ,   availableWorkers( other.availableWorkers )
    ,   pendingClientRequests( std::move( other.pendingClientRequests ) )
    ,   requests( std::move( other.requests ) )
    {
        other.totalWorkers = 0;
        other.requestType = 0;
    }

    BrokerRequestTypeContext::~BrokerRequestTypeContext()
    {
    }

    RequestState* BrokerRequestTypeContext::AddInProgressRequest(
        RequestId requestId,
        const LHCVersionFlags& vfs,
        WorkerInfo* workerInfo )
    {
        auto ret = requests.emplace(
                requestId,
                std::unique_ptr< RequestState >(
                    new RequestState( requestId, vfs, workerInfo ) ) );
        // assert new insert it->second == true
        return ret.first->second.get();
    }

    void BrokerRequestTypeContext::RemoveInProgressRequest( RequestId id )
    {
        // assert assigned/in progress
        requests.erase( id );
    }

    void BrokerRequestTypeContext::AddPendingRequest(
        RequestId requestId,
        const LHCVersionFlags& vfs,
        ZMQMessage** lpRequestMsg )
    {
        auto ret = requests.insert(
            std::pair< RequestId, std::unique_ptr< RequestState > >(
                requestId,
                std::unique_ptr< RequestState >(
                    new RequestState( requestId, vfs, lpRequestMsg ) ) ) );
        // assert new insert
        RequestState* requestState( ret.first->second.get() );
        auto it = pendingClientRequests.insert( pendingClientRequests.end(), requestState );
        // circular relationship will allow for priority of a request to be changed
        // just from a reference to the RequestState and pending request list
        requestState->SetQueueCursor( it );
    }

    void BrokerRequestTypeContext::BackToPending( RequestState* requestState )
    {
        // assert assigned and in progress
        auto it = pendingClientRequests.insert( pendingClientRequests.end(), requestState );
        requestState->SetQueueCursor( it );
    }

    void BrokerRequestTypeContext::AddAvailableWorker( WorkerInfo* workerInfo )
    {
        availableWorkers.insert( workerInfo );
        // assert new insert
    }
    
    void BrokerRequestTypeContext::RemoveAvailableWorker( WorkerInfo* workerInfo )
    {
        availableWorkers.erase( workerInfo );
    }

    RequestState* BrokerRequestTypeContext::GetNextPendingRequest( )
    {
        return
            pendingClientRequests.size()
            ? pendingClientRequests.front()
            : nullptr;
    }

    const RequestState*
    BrokerRequestTypeContext::GetNextPendingRequest( ) const
    {
        return
            pendingClientRequests.size()
            ? pendingClientRequests.front()
            : nullptr;
    }

    WorkerInfo* BrokerRequestTypeContext::GetNextAvailableWorker( )
    {
        auto it = availableWorkers.begin();
        return it != availableWorkers.end() ? *it : nullptr;
    }

    const WorkerInfo* BrokerRequestTypeContext::GetNextAvailableWorker( ) const
    {
        auto it = availableWorkers.begin();
        return it != availableWorkers.end() ? *it : nullptr;
    }

    BrokerRequestTypeContext::NumAvailWorkersType
    BrokerRequestTypeContext::NumWorkersAvailable() const
    {
        return availableWorkers.size();
    }

    BrokerRequestTypeContext::NumRequestsPendingType
    BrokerRequestTypeContext::NumRequestsPending() const
    {
        return pendingClientRequests.size();
    }

    void BrokerRequestTypeContext::IncrementNumWorkers( )
    {
        ++totalWorkers;
    }

    void BrokerRequestTypeContext::DecrementNumWorkers( )
    {
        --totalWorkers;
    }

    int BrokerRequestTypeContext::NumWorkersTotal( ) const
    {
        return totalWorkers;
    }

    void BrokerRequestTypeContext::AssignWorkerFromPending(
        RequestState* requestState,
        WorkerInfo* workerInfo )
    {
        pendingClientRequests.erase( requestState->GetQueueCursor() ); 
        requestState->Assign( workerInfo );
    }

    void BrokerRequestTypeContext::AssignToOtherWorker(
        RequestState* requestState,
        WorkerInfo* workerInfo )
    {
        requestState->Assign( workerInfo );
    }

    RequestState* BrokerRequestTypeContext::GetRequestState(
        RequestId requestId )
    {
        auto it = requests.find( requestId );
        return ( it != requests.end() ) ? it->second.get() : nullptr;
    }

    const RequestState* BrokerRequestTypeContext::GetRequestState(
        RequestId requestId ) const
    {
        auto it = requests.find( requestId );
        return ( it != requests.end() ) ? it->second.get() : nullptr;
    }
}
}
