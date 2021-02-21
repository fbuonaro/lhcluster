#ifndef __LHCLUSTER_IMPL_WORKER_H__
#define __LHCLUSTER_IMPL_WORKER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/iworker.h>
#include <lhcluster_impl/requestprocessor.h>
#include <lhcluster_impl/workerrequesttypecontext.h>
#include <lhcluster_impl/clientrequestcontext.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>
#include <lhcluster_stats/workerstats.h>

#include <thread>
#include <memory>
#include <map>
#include <set>

namespace LHClusterNS
{
namespace Impl
{
    enum class WorkerExitState
    {
        None,
        GracefulStop,
        BrokerDied,
        ProcessorInaccessable,
        ReceivedUnsupportedRequest,
        ProcessorFatalError,
        StartFailed
    };

    struct WorkerConstructionFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class Worker : public IWorker
    {
        public:
            // DATA MEMBERS
            // number of requests that this worker can handle
            unsigned int capacity;
            // state member stores the circumstances under
            // which the reactor loop exited
            WorkerExitState exitState;
            // the current time
            std::time_t currentTime;
            // addresses of various endpoints
            // address which messages are received on/sent from
            Endpoint selfMessagingEndpoint;   
            // address of the broekr which messages are sent to/received from
            Endpoint brokerMessagingEndpoint; 
            // address of the socket which receives various signals form the 
            // primary thread
            Endpoint selfControllerEndpoint;  
            // sockets
            // socket used to communicate with Workers - heartbeats are sent,
            SocketDealer* messagingSocket;  
            // socket used by secondary thread to receive commands from primary thrd
            SocketPair* listenerSocket;   
            // socket used by primary thread
            SocketPair* signalerSocket;   
            // socket for communication with request processor
            SocketPair* requestProcessorMessagingSocket;
            // zmq contextual structures
            // loops over various sockets and timers, triggering 
            // approriate callbacks
            ZMQReactor* reactor;                
            // ids of reactor triggers
            // numeric id of the send heartbeat callback
            int heartbeatBrokerReactorId;    
            // numeric id of the check pulse of broker callback
            int checkPulseBrokerReactorId;  
#ifdef STATS
            // id of the timer used to dump stats
            int dumpStatsReactorId;
#endif
            // pointer to secondary thread which the reactor runs in
            std::unique_ptr<std::thread> reactorThread;    
            // delay between sending heartbeats/other comm to the broker
            // initialized to a default value but can be reset by broker
            // via a broker heartbeat or initiali handshake
            Delay brokerHeartbeatDelay;   
            // delay between receiving heartbeats from broker, sent w/ HB
            Delay selfHeartbeatDelay;     
            // delay between checking if the broker is still alive
            Delay brokerPulseDelay;       
            // length of inactivity required before a broker can be considered dead
            Delay brokerDeathDelay;       
            // when a message from the broker was last received
            time_t brokerLastActive;             
            // when this worker was last active, updated with each message
            time_t workerLastActive;             
            // context of each supported request type
            std::map< RequestType, WorkerRequestTypeContext > requestTypeContexts; 
            // context of each  individual request
            std::map< std::pair< RequestType, RequestId >, ClientRequestContext >
            requestContexts; 
            // request processor
            RequestProcessor requestProcessor;

#ifdef STATS
            MessageStats< WorkerStat > stats;
#endif
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;

            // CTORS, DTORS
            Worker( const Worker& other ) = delete;
            Worker( Worker&& other ) = delete;
            Worker& operator=( const Worker& other ) = delete;
            Worker& operator=( Worker&& other ) = delete;
            Worker() = delete;

            Worker( const Endpoint& _selfMessagingEndpoint, 
                    const Endpoint& _brokerMessagingEndpoint,
                    const Endpoint& _selfControllerEndpoint,
                    Delay _selfHeartbeatDelay,
                    Delay _initialBrokerHeartbeatDelay,
                    Delay _brokerPulseDelay,
                    std::unique_ptr< IZMQSocketFactory > _socketFactory,
                    std::unique_ptr< IZMQSocketSenderReceiver > _senderReceiver,
                    const Endpoint& _requestProcessorMessagingEndpoint,
                    const Endpoint& _requestProcessorControllerEndpoint,
                    std::unique_ptr< IRequestHandler > _requestHandler,
                    std::unique_ptr< IZMQSocketFactory > _requestProcessorSocketFactory,
                    std::unique_ptr< IZMQSocketSenderReceiver > _requestProcessorSenderReceiver );
            ~Worker();

            // METHODS
            // IWorker
            // start polling incoming messages in a secondary thread
            void Start();               
            // IWorker
            // start polling incoming messages in this thread
            void StartAsPrimary();    
            // IWorker
            // stop polling incoming messages
            void Stop();                
            void setExitState( WorkerExitState newExitState );
            // sends the heartbeat over the dealer socket to the broker
            int sendBrokerHeartbeat();    
            // parse a message from the broker received over the dealer socket
            int handleBrokerMessage( ZMQMessage** lpDealerMessage );    
            int handleBrokerMessage();    
            // check the pulse of the broker, 
            // if the broker has died, the reactor loop 
            // will be exited and all connections
            // restarted
            int checkBrokerPulse();       
            // handle a heartbeat message from a broker,
            // doing basic validation and 
            // updating the broker as appropriate
            int handleBrokerHeartbeat( ZMQMessage* heartbeatMsg );    
            // send the broker the standard "READY" for work message
            int sendBrokerReadyMessage();            
            // setup all callbacks for the messaging sockets
            int setupMessagingCallbacks();    
            // setup all callbacks for the request handler sockets
            int setupProcessorCallbacks();      
            // setup all callbacks for the controller sockets
            int setupControllerCallbacks();   
            // remove all callbacks from the messaging sockets
            int teardownMessagingCallbacks(); 
            // remove all callbacks functions from rhandler related sockets
            int teardownProcessorCallbacks();   
            // remove all callbacks from the primary/secondary thread sockets
            int teardownControllerCallbacks();
            // connect and bind sockets for primary/secondary threads
            int activateControllerSockets();          
            // disconnect and unbind sockets for primary/secondary threads
            int deactivateControllerSockets();        
            // connect and bind sockets related to messaging
            int activateMessagingSockets();           
            // disconnect and unbind sockets related to messaging
            int deactivateMessagingSockets();         
            // update when the worker was last active
            void updateWorkerLastActive( std::time_t activeTime );  
            // update when the broker was last active
            void updateBrokerLastActive( std::time_t activeTime );  
            // returns true if a heartbeat should be sent to the broker
            bool heartbeatNeededAsOf(std::time_t asofTime ); 
            // update the current system time
            void updateCurrentTime();                         
            // return true if the broker was alive as of the input time
            bool isBrokerAliveAsOf( std::time_t asofTime ); 
            // parse a message from the request handler socket
            int handleProcessorMessage( ZMQMessage** lpProcessorMessage );
            int handleProcessorMessage();                               
            // handle a new request from the broker,
            // doing basic validation and passing it to the
            // request handler or elsewhere as appropriate
            int handleBrokerNewRequest( ZMQMessage** newRequestMsg );  
            // initialize the local datastructures which
            // store the WorkerRequestTypeContexts for this
            // worker
            void initializeRequestTypeContexts();                     
            void addNewInProgressRequest( RequestType type, RequestId id );
            bool requestRedirected( ClientRequestContext& requestContext );
#ifdef STATS
            int dumpStatsHeaders();
            int dumpStats();
#endif
    };
}
}


#endif
