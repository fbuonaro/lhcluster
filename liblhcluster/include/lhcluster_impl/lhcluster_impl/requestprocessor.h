#ifndef __LHCLUSTER_IMPL_REQUESTPROCESSOR_H__
#define __LHCLUSTER_IMPL_REQUESTPROCESSOR_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/irequesthandler.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>

#include <exception>
#include <thread>

namespace LHClusterNS
{
namespace Impl
{
    // Exception thrown when construction fails
    class RequestProcessorCreateFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class RequestProcessorActivateFailed : public std::exception
    {
        virtual const char* what() const throw();
    };

    class RequestProcessor
    {
        public:
            // DATA
            // endpoint of self request handler, handler listens on this addr
            Endpoint selfMessagingEndpoint;
            // endpoint of primary/secondary thread messaging, secondary
            // thread listens on this addr
            Endpoint selfControllerEndpoint; 
            // socket of request handler, used to send responses and send
            // requests
            SocketPair* selfMessagingSocket;
            // reactor loop
            ZMQReactor* reactor;
            // socket of secondary thread to listen for commands from primary
            SocketPair* selfSecondarySocket;
            // socket of primary thread to send commands to secondary thread
            SocketPair* selfPrimarySocket;
            // secondary thread
            std::unique_ptr< std::thread > reactorThread;
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;
            std::unique_ptr< IRequestHandler > requestHandler;

            //CTORS, DTOR
            RequestProcessor() = delete;
            RequestProcessor( const RequestProcessor& other ) = delete;
            RequestProcessor( RequestProcessor&& other ) = delete;
            RequestProcessor& operator=( RequestProcessor& other ) = delete;

            RequestProcessor( const Endpoint& _selfMessagingEndpoint, 
                              const Endpoint& _selfControllerEndpoint,
                              std::unique_ptr< IZMQSocketFactory >
                                  _socketFactory,
                              std::unique_ptr< IZMQSocketSenderReceiver > 
                                  _senderReceiver,
                              std::unique_ptr< IRequestHandler > _requestHandler );
            ~RequestProcessor();

            // METHODS
            const std::vector< RequestType >& SupportedRequestTypes() const;
            // start the reactor loop in a secondary thread
            void Start();
            // stop the reactor loop in the secondary thread
            void Stop();
            // handle a request from the worker
            int handleRequest( ZMQMessage** lpRequestMsg );
            int handleRequest();
            // setup reactors for worker/request handler messages
            int setupMessagingCallbacks();
            int teardownMessagingCallbacks();
            // setup reactors for primary/secondary thread messages
            int setupControllerCallbacks();
            int teardownControllerCallbacks();
            // setup messaging sockets
            int activateMessagingSockets();
            int deactivateMessagingSockets();
            // setup controller sockets
            int activateControllerSockets();
            int deactivateControllerSockets();
            // start the reactor loop
            // will block until the reactor loop exits
            void StartAsPrimary();
    };
}
}

#endif
