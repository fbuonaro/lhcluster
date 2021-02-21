#ifndef __LHCLUSTER_IMPL_BROKERREQUESTTYPECONTEXT_H__
#define __LHCLUSTER_IMPL_BROKERREQUESTTYPECONTEXT_H__


#include <lhcluster/cluster.h>
#include <lhcluster_impl/workerinfo.h>
#include <lhcluster_impl/requeststate.h>
#include <lhcluster_impl/zmqaddin.h>
#include <lhcluster_stats/requesttypestats.h>

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <memory>


namespace LHClusterNS
{
namespace Impl
{
    // Contains information regarding the current state of specific request type
    class BrokerRequestTypeContext
    {
        public:
            typedef std::unordered_set< WorkerInfo* >::size_type NumAvailWorkersType;
            typedef std::list< RequestState* >::size_type NumRequestsPendingType;


            //DATA MEMBERS
#ifdef STATS
            MessageStats< RequestTypeStat > stats;
#endif

            //CTORS, DTORS
            BrokerRequestTypeContext( const BrokerRequestTypeContext& ) = delete;
            BrokerRequestTypeContext& operator=( const BrokerRequestTypeContext& ) = delete;
            BrokerRequestTypeContext& operator=( BrokerRequestTypeContext&& ) = delete;

            BrokerRequestTypeContext( RequestType _requestType );
            BrokerRequestTypeContext( BrokerRequestTypeContext&& wrtc );
            ~BrokerRequestTypeContext();

            //METHODS
            NumAvailWorkersType NumWorkersAvailable( ) const;
            NumRequestsPendingType NumRequestsPending( ) const;
            int NumWorkersTotal( ) const;
            void IncrementNumWorkers();
            void DecrementNumWorkers();
            RequestState* AddInProgressRequest(
                RequestId requestId,
                const LHCVersionFlags& vfs,
                WorkerInfo* workerInfo );
            void RemoveInProgressRequest( RequestId requestId );
            void BackToPending( RequestState* requestState );
            void AddPendingRequest(
                RequestId requestId,
                const LHCVersionFlags& vfs,
                ZMQMessage** lpRequestMsg );
            void AddAvailableWorker( WorkerInfo* workerInfo );
            void RemoveAvailableWorker( WorkerInfo* workerInfo );
            RequestState* GetNextPendingRequest();
            const RequestState* GetNextPendingRequest() const;
            WorkerInfo* GetNextAvailableWorker();
            const WorkerInfo* GetNextAvailableWorker() const;
            RequestState* GetRequestState( RequestId requestId );
            const RequestState* GetRequestState( RequestId requestId ) const;
            void AssignToOtherWorker(
                RequestState* requestState,
                WorkerInfo* workerInfo );
            void AssignWorkerFromPending(
                RequestState* requestState,
                WorkerInfo* workerInfo );
            RequestType GetRequestType() const
            {
                return requestType;
            }

        private:
            //DATA MEMBERS
            int totalWorkers;
            RequestType requestType;
            // workers immediately available to work on a request
            // this does not own the workerInfo objcts,
            // assume that they are proeprly maintained somewhere else
            std::unordered_set< WorkerInfo* > availableWorkers;
            // Requests which are waiting to be srviced
            // TODO - priority lists by ordered map from priority -> list
            // RequestState would have a priority member 
            // and priority would give the list via queuesByPriority[ priority ] 
            // and reprioritization would be move from one list to another
            std::list< RequestState* > pendingClientRequests;
            // owns ZMQMessage* in each RequestState which is pending
            std::unordered_map< RequestId, std::unique_ptr< RequestState > > requests;
    };
}
}

#endif
