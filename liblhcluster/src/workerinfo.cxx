#include <lhcluster_impl/workerinfo.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

//Best guess at what the size of a frame is likely to be 
#define MSG_SIZE_BEST_GUESS 32
namespace LHClusterNS
{
namespace Impl
{
    WorkerInfo::WorkerInfo()
    :   freeCapacity( 0 )
    ,   identity( nullptr )
    ,   brokerLastActive( 0 )
    ,   workerLastActive( 0 )
    ,   heartbeatSendDelay( 0 )
    ,   inProgressRequests()
    ,   vfs()
    {
    }

    // msg should have the form:
    //  <Request Capacity> | <Time Elapse till Heartbeat> ( | <RequestType> )+
    // ownership of msg is passed to the constructor
    // destruction guaranteed whether or not ctor succeeds
    WorkerInfo::WorkerInfo(
        ZMQFrame** lpWorkerReadyIdentity,
        time_t receivedAtTime,
        unsigned int _freeCapacity,
        std::vector< RequestType > _supportedRequestTypes,
        Delay _heartbeatSendDelay,
        const LHCVersionFlags& _vfs )
    :   freeCapacity( _freeCapacity )
    ,   supportedRequestTypes( move( _supportedRequestTypes ) )
    ,   identity( *lpWorkerReadyIdentity )
    ,   brokerLastActive( receivedAtTime )
    ,   workerLastActive( receivedAtTime )
    ,   heartbeatSendDelay( _heartbeatSendDelay )
    ,   inProgressRequests()
    ,   vfs( _vfs )
    {
        *lpWorkerReadyIdentity = nullptr;
    }

    WorkerInfo& WorkerInfo::operator=( WorkerInfo&& wi )
     {
        freeCapacity = wi.freeCapacity;
        supportedRequestTypes = std::move( wi.supportedRequestTypes );
        brokerLastActive = wi.brokerLastActive;
        workerLastActive = wi.workerLastActive;
        heartbeatSendDelay = wi.heartbeatSendDelay;
        inProgressRequests = std::move( wi.inProgressRequests );

        if( identity )
            zframe_destroy( &identity );

        identity = wi.identity;
        wi.identity = nullptr;

        vfs = wi.vfs;

        return *this;
    }

    WorkerInfo::WorkerInfo( WorkerInfo&& wi )
    :   freeCapacity( wi.freeCapacity )
    ,   supportedRequestTypes( std::move( wi.supportedRequestTypes ) )
    ,   identity( wi.identity )
    ,   brokerLastActive( wi.brokerLastActive )
    ,   workerLastActive( wi.workerLastActive )
    ,   heartbeatSendDelay( wi.heartbeatSendDelay )
    ,   inProgressRequests( std::move( wi.inProgressRequests ) )
    ,   vfs( wi.vfs )
    {
        wi.identity = nullptr;
    }

    WorkerInfo::~WorkerInfo()
    {
        if( identity )
            zframe_destroy( &identity );
    }

    bool WorkerInfo::IsAliveAsOf( std::time_t asOfTime, Delay deathDelay )
    {
        bool ret = false;
        struct tm workerLastActiveTm = {0};
        std::time_t dead_at = 0;

        localtime_r( &workerLastActive, &workerLastActiveTm );

        workerLastActiveTm.tm_sec += deathDelay.count();
        dead_at = mktime( &workerLastActiveTm );

        if( dead_at > asOfTime )
        {
            ret = true;
        }
        
        return ret;
    }

    bool WorkerInfo::IsHeartbeatNeededAsOf( std::time_t asOfTime )
    {
        bool ret = false;
        struct tm brokerLastActiveTm = {0};
        std::time_t heartbeat_at = 0;

        localtime_r( &brokerLastActive, &brokerLastActiveTm );

        brokerLastActiveTm.tm_sec += heartbeatSendDelay.count();
        heartbeat_at = mktime( &brokerLastActiveTm );

        if( heartbeat_at <= asOfTime )
            ret = true;
        
        return ret;
 
    }

    void WorkerInfo::UpdateWithHeartbeat(
            std::time_t receivedAtTime,
            std::time_t sentAt,
            Delay newHeartbeatSendDelay )
    {
        UpdateWorkerLastActive( receivedAtTime );
        sentAt = 0; // unused
        heartbeatSendDelay = newHeartbeatSendDelay;
    }

    void WorkerInfo::UpdateBrokerLastActive( std::time_t asOfTime )
    {
        brokerLastActive = asOfTime;
    };

    void WorkerInfo::UpdateWorkerLastActive( std::time_t asOfTime )
    {
        workerLastActive = asOfTime;
    };

    void WorkerInfo::AddInProgressRequest(
        RequestType requestType, 
        RequestState* requestState )
    {
        auto it = inProgressRequests.find( requestType );
        if( it == inProgressRequests.end() )
            it = inProgressRequests.emplace(
                requestType, std::unordered_map< RequestId, RequestState* >() ).first;
        it->second.emplace( requestState->GetRequestId(), requestState );
        --freeCapacity;
    }

    void WorkerInfo::RemoveInProgressRequest( RequestType requestType, RequestId requestId )
    {
        auto it = inProgressRequests.find( requestType );
        if( it != inProgressRequests.end() )
            it->second.erase( requestId );
        ++freeCapacity;
    }
}
}
