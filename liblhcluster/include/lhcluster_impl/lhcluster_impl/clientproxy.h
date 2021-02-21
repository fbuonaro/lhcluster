#ifndef __LHCLUSTER_IMPL_CLIENTPROXY_H__
#define __LHCLUSTER_IMPL_CLIENTPROXY_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/iclientproxy.h>
#include <lhcluster_impl/clientrequestbuffer.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>
#include <lhcluster_stats/clientproxystats.h>

#include <thread>
#include <memory>

namespace LHClusterNS
{
namespace Impl
{

    enum class ClientProxyExitState
    {
        None,
        GracefulStop,
        BrokerDied,
        StartFailed
    };

    struct ClientProxyConstructionFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class ClientProxy : public IClientProxy
    {
        public:
            // DATA MEMBERS
            // Buffer of requests sent to the broker, but pending a 
            // receipt of processing
            ClientRequestBuffer requestsBuffer;
            // state member stores the circumstances under 
            // which the reactor loop exited
            ClientProxyExitState exitState;
            // the current time
            std::time_t currentTime;
            // addresses of various endpoints
            // address which client requests are received from and replies
            // sent to clients from
            Endpoint selfClientMessagingEndpoint;
            // address which requests are sent from/replies 
            // listened for on
            Endpoint selfBrokerMessagingEndpoint;
            // address which broker listens for requests on/sends replies frm
            Endpoint brokerMessagingEndpoint;
            // address of the socket which receives various signals form the 
            // primary thread
            Endpoint selfControllerEndpoint;
            // sockets
            // socket used to communicate with the various clients
            SocketRouter* clientMessagingSocket;    
            // socket used to communicate with the Broker
            SocketDealer* brokerMessagingSocket;    
            // socket used by secondary thread to receive commands from primary
            SocketPair* listenerSocket;            
            // socket used by primary thread
            SocketPair* signalerSocket;            
            // zmq contextual structures
            // loops over various sockets and timers, 
            // triggering approriate callbacks
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
            // when this client was last active, updated with each message
            time_t clientLastActive;             
#ifdef STATS
            MessageStats< ClientProxyStat > stats;
#endif
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;

            // CTORS, DTORS
            ClientProxy( const ClientProxy& other ) = delete;
            ClientProxy( ClientProxy&& other ) = delete;
            ClientProxy& operator=( const ClientProxy& other ) = delete;
            ClientProxy& operator=( ClientProxy&& other ) = delete;
            ClientProxy() = delete;

            ClientProxy( const Endpoint& _selfBrokerMessagingEndpoint, 
                         const Endpoint& _brokerMessagingEndpoint,
                         const Endpoint& _selfControllerEndpoint,
                         const Endpoint& _selfClientMessagingEndpoint,
                         Delay _selfHeartbeatDelay,
                         Delay _initialBrokerHeartbeatDelay,
                         Delay _brokerPulseDelay,
                         std::unique_ptr< IZMQSocketFactory > _socketFactory,
                         std::unique_ptr< IZMQSocketSenderReceiver > 
                             _senderReceiver );

            ~ClientProxy();

            // METHODS
            // IClientProxy
            // start polling incoming messages in a secondary thread
            void Start();
            // IClientProxy
            // start polling incoming messages in this thread
            void StartAsPrimary();
            // IClientProxy
            // stop polling incoming messages
            void Stop();
            void setExitState( ClientProxyExitState newExitState );
            // parse a message from the broker received over the dealer socket
            int handleBrokerMessage( ZMQMessage** lpDealerMsg );    
            int handleBrokerMessage();    
            // sends the heartbeat over the dealer socket to the broker
            int sendBrokerHeartbeat();    
            // check the pulse of the broker, 
            // if the broker has died, the reactor 
            // loop will be exited and all connections
            // restarted
            int checkBrokerPulse();       
            // setup all callbacks for the messaging sockets
            int setupBrokerMessagingCallbacks();     
            // setup all callbacks for the messaging sockets
            int setupClientMessagingCallbacks();     
            // setup all callbacks for the controller sockets
            int setupControllerCallbacks();           
            // remove all callbacks from the messaging sockets
            int teardownBrokerMessagingCallbacks();  
            // remove all callbacks from the messaging sockets
            int teardownClientMessagingCallbacks();  
            // remove all callbacks from the primary/secondary thread sockets
            int teardownControllerCallbacks();        
            // connect and bind sockets for primary/secondary threads
            int activateControllerSockets();          
            // disconnect and unbind sockets for primary/secondary threads
            int deactivateControllerSockets();        
            // connect and bind sockets related to messaging
            int activateClientMessagingSockets();    
            // disconnect and unbind sockets related to messaging
            int deactivateClientMessagingSockets();  
            // connect and bind sockets related to messaging
            int activateBrokerMessagingSockets();    
            // disconnect and unbind sockets related to messaging
            int deactivateBrokerMessagingSockets();  
            // update when the client was last active
            void updateClientLastActive( std::time_t activeTime );  
            // update when the broker was last active
            void updateBrokerLastActive( std::time_t activeTime );  
            // returns true if a heartbeat should be sent to the broker
            bool heartbeatNeededAsOf(std::time_t asOfTime ); 
            // update the current system time
            void updateCurrentTime();                         
            // return true if the broker was alive as of the input time
            bool isBrokerAliveAsOf( std::time_t asOfTime ); 
            // handle a client request request should include
            int handleClientRequest(
                ZMQFrame* identity,
                ZMQMessage** lprequestMsg );    
            // parse a message from the broker received over the 
            // dealer socket
            int handleBrokerResponse( ZMQMessage** lpresponseMsg );      
            // handle a heartbeat message from a broker,
            // doing basic validation and 
            // updating the broker as appropriate
            int handleBrokerHeartbeat( ZMQMessage** lpheartbeatMsg );    
            // handle message from client
            int handleClientMessage( ZMQMessage** lpRouterMsg );
            int handleClientMessage();                                
            // receipt
            int handleBrokerReceipt(
                ClusterMessage receiptType, ZMQMessage** lpreceiptMsg );        
#ifdef STATS
            int dumpStatsHeaders();
            int dumpStats();
#endif
    };
}
}

#endif
