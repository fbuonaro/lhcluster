#ifndef __LHCLUSTER_IMPL_WORKERINFO_H__
#define __LHCLUSTER_IMPL_WORKERINFO_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/requeststate.h>
#include <lhcluster_impl/zmqaddin.h>

#include <ctime>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

// The grace period is the time allowed for a heartbeat or
// other activity to be received after one is sent out
#define GRACE_PERIOD_MULTIPLIER 3

namespace LHClusterNS
{
namespace Impl
{
    class WorkerInfo
    {
        public:
            // CTORS, DTORS
            WorkerInfo( const WorkerInfo& ) = delete;
            WorkerInfo& operator=( const WorkerInfo& wi ) = delete;

            WorkerInfo();
            WorkerInfo( ZMQFrame** lpWorkerReadyIdentity,
                        time_t receivedAtTime,
                        unsigned int freeCapacity,
                        std::vector< RequestType > supportedRequestTypes,
                        Delay heartbeatSendDelay,
                        const LHCVersionFlags& vfs );
            ~WorkerInfo();
            WorkerInfo( WorkerInfo&& wi );
            WorkerInfo& operator=( WorkerInfo&& wi );

            // METHODS
            const ZMQFrame* GetIdentity() const
            {
                return identity;
            }

            std::time_t GetBrokerLastActive() const
            {
                return brokerLastActive;    
            }

            std::time_t GetWorkerLastActive() const 
            {
                return workerLastActive;
            }

            const std::vector< RequestType >&
            GetSupportedRequestTypes() const
            {
                return supportedRequestTypes;
            }

            Delay GetHeartbeatSentDelay() const
            {
                return heartbeatSendDelay;
            }

            int FreeCapacity() const
            {
                return freeCapacity;
            }

            const
            std::unordered_map<
                RequestType,
                std::unordered_map< RequestId, RequestState* > >&
            GetInProgressRequests() const
            {
                return inProgressRequests;
            }

            const LHCVersionFlags& GetVersionAndFlags() const
            {
                return vfs;
            }

            bool IsOtherBroker() const
            {
                return vfs.hasAll( MessageFlag::Broker );
            }

            bool IsAliveAsOf( std::time_t asOfTime, Delay deathDelay );
            bool IsHeartbeatNeededAsOf( std::time_t asOfTime );
            void UpdateBrokerLastActive( std::time_t asOfTime );
            void UpdateWorkerLastActive( std::time_t asOfTime );
            void AddInProgressRequest(
                RequestType requestType,
                RequestState* requestState );
            void RemoveInProgressRequest( RequestType type, RequestId id );
            void UpdateWithHeartbeat( std::time_t receivedAt,
                                      std::time_t sentAt,
                                      Delay newHeartbeatSendDelay );

        private:
            // DATA MEMBERS
            // How many more requests this worker can take on
            unsigned int freeCapacity;
            // The request types supported by this worker
            std::vector< RequestType > supportedRequestTypes;
            ZMQFrame* identity;
            // When some communcication was last sent to this worker
            time_t brokerLastActive;
            // When a message from the worker was last received
            time_t workerLastActive;
            // The time that should elapse between brokerLastActive
            // and the next heartbeat to the worker
            Delay heartbeatSendDelay;
            std::unordered_map<
                RequestType,
                std::unordered_map< RequestId, RequestState* > > inProgressRequests;
            LHCVersionFlags vfs;
    };
}
}

#endif
