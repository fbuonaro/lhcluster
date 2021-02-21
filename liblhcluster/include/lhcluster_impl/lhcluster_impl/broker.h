#ifndef __LHCLUSTER_IMPL_BROKER_H__
#define __LHCLUSTER_IMPL_BROKER_H__

#include <lhcluster/ibroker.h>
#include <lhcluster/endpoint.h>
#include <lhcluster_impl/workerinfolookup.h>
#include <lhcluster_impl/brokerrequesttypecontext.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>
#include <lhcluster_stats/brokerstats.h>

#include <thread>

namespace LHClusterNS
{
namespace Impl
{
    struct BrokerConstructionFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class Broker : public IBroker
    {
        typedef
            std::unordered_map< const RequestType, BrokerRequestTypeContext >
            BrokerRequestTypeContextLookup;

        public:
            // DATA MEMBERS
#ifdef STATS
            MessageStats< BrokerStat > stats;
#endif
            // addresses
            // address of router socket which communicates with "workers"
            Endpoint selfBackendMessagingEndpoint;
            // address of router socket which communicates with "clients"
            Endpoint selfFrontendMessagingEndpoint;
            // address of pair socket used for primary/seconday thread
            // communication and synchronization
            Endpoint selfControllerEndpoint;
            // sockets
            // socket used to communicate with "workers"
            SocketRouter* backendMessagingSocket;   
            // socket used to communicate with "clients"
            SocketRouter* frontendMessagingSocket;
            // secondary thread's half of the primary/secondary thread
            SocketPair* listenerSocket;
            // primary thread's half of the requester/worker pair of sockets
            SocketPair* signalerSocket;
            WorkerInfoLookup workerInfoLookup;
            ZMQReactor* reactor;
            // the time that should elapse between when a worker was
            // last active and the proceeding heartbeat to the broker
            Delay selfHeartbeatDelay;
            // the time that should elapse between when a worker was last
            // active and when it is considered dead
            Delay workerDeathDelay;
            // the minimum requested heartbeat delay from a worker, used to 
            // determine the delay for the heartbeat reactor timer
            Delay minimumWorkerHeartbeatDelay;
            // time between checking each worker's pulse
            Delay workerPulseDelay;
            // ids of reactor triggers, used to stop or change the reactor triggers
            // id of the worker heartbeat reactor timer
            int heartbeatWorkerReactorId;
            // id of the timer used to check the pulse of each worker
            int checkPulseWorkerReactorId;
#ifdef STATS
            // id of the timer used to dump stats
            int dumpStatsReactorId;
#endif
            // the current time is referenced so much that it 
            // just makes sense to cache it and update it when needed
            time_t currentTime;
            // pointer to secondary thread which the reactor runs in
            std::unique_ptr<std::thread> reactorThread;
            BrokerRequestTypeContextLookup requestTypeContextLookup;
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;

            // CTORS, DTORS
            Broker( const Broker& other ) = delete;
            Broker& operator=( const Broker& other ) = delete;
            Broker( Broker&& other ) = delete;
            Broker() = delete;

            Broker( const Endpoint& _selfFrontendMessagingEndpoint,
                    const Endpoint& _selfBackendMessagingEndpoint,
                    const Endpoint& _selfControllerEndpoint, 
                    Delay _selfHeartbeatDelay,
                    Delay _minimumWorkerHeartbeatDelay,
                    Delay _workerPulseDelay,
                    std::unique_ptr< IZMQSocketFactory > _socketFactory,
                    std::unique_ptr< IZMQSocketSenderReceiver > 
                        _senderReceiver );
            ~Broker();

            // METHODS
            // IBroker
            // start polling incoming messages
            void Start();
            // IBroker
            // stop polling incoming messages
            void Stop();                
            // IBroker
            // start polling in the current thread,
            // taking control of the thread until canceled
            void StartAsPrimary(); 
            // send heartbeats to all workers which require it
            int sendWorkerHeartbeats(); 
            // send heartbeat to single worker
            int sendWorkerHeartbeat( WorkerInfo* workerinfo ); 
            // check the pulse of each worker, disconnecting the dead workers
            int checkWorkersPulse();      
            // activate the various sockets related to managing worker
            int activateControllerSockets();      
            // deactivate and disconnect the various sockets for managing
            int deactivateControllerSockets();    
            // treardown the reactor callback functions for ocntroller
            int teardownControllerCallbacks();    
            // treardow the reactor callback functions for ocntroller
            int setupControllerCallbacks();       
            // activate the various sockets related to cluster
            int activateMessagingSockets();       
            // deactivate and disconnect various sockets for cluster
            int deactivateMessagingSockets();     
            // setup the reactor callback functions for messaging sockets
            int setupMessagingCallbacks();    
            // teradown messagignn callbacls sockets
            int teardownMessagingCallbacks(); 
            // handles a message on the backend socket, probably from worker
            int handleBackendMessage();       
            int handleBackendMessage( ZMQMessage** lpBackendMessage );
            // handle worker heartbeat
            int handleWorkerHeartbeat(
                ZMQFrame* identity,
                ZMQMessage** heartbeat_msg );
            // handle a worker ready, from new or existing worker
            int handleWorkerReady( ZMQFrame** lpIdentity, ZMQMessage** readyMsg );
            int handleWorkerDeath( WorkerInfo* workerinfo );
            int updateContextsForDeadWorker( WorkerInfo* workerinfo );
            int updateContextsForNewWorker( WorkerInfo* workerinfo );
            int updateContextsForFinishedWorker(
                WorkerInfo* workerinfo,
                RequestType type,
                RequestId id );
            int handleWorkerFinished(
                ZMQFrame** lpIdentity,
                ZMQMessage** finishedMsg );
            int handleClientRequest( ZMQMessage** lpNewRequest );
            int forwardParsedClientRequest(
                const LHCVersionFlags& vfs,
                RequestType type,
                RequestId id,
                ZMQMessage** lpNewRequest );
            int forwardWorkerParsedClientRequest(
                WorkerInfo* workerinfo,
                RequestId id,
                const LHCVersionFlags& vfs,
                ZMQMessage** lpNewRequest,
                BrokerRequestTypeContext& requestTypeContext );
            int forwardWorkerParsedClientRequest(
                WorkerInfo* workerinfo, 
                BrokerRequestTypeContext& requestTypeContext,
                RequestState* requestState );
            int handleFrontendMessage( ZMQMessage** frontendMessage );
            int handleFrontendMessage();
            int handleClientHeartbeat( ZMQMessage** lpClientHeartbeat );
            int handleWorkerReceipt( ZMQFrame* identityFrame, ZMQMessage** lpWorkerReceipt );
            void updateCurrentTime();
#ifdef STATS
            int dumpStatsHeaders();
            int dumpStats();
#endif
            int sendReceipt(
                ZMQMessage** lpSendReceiptMsg, RequestType requestType, RequestId requestId );
            int handleCopiedRequest(
                RequestState* requestState,
                const LHCVersionFlags& vfs,
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpCopiedRequest );
            ZMQMessage* copyRequestForFlags( const LHCVersionFlags& vfs, ZMQMessage* newRequest );
            int notifyOnRedirect( RequestType requestType, RequestState* requestState );
            int rejectRequest(
                ZMQMessage** lpNewRequest,
                RequestStatus requestStatus );
            int rejectRequest(
                const LHCVersionFlags& vfs,
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpNewRequest,
                RequestStatus requestStatus );
    };
}
}

#endif
