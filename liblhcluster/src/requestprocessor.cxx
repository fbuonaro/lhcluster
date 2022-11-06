#include <lhcluster_impl/requestprocessor.h>
#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

namespace LHClusterNS
{
    namespace Impl
    {
        const char* RequestProcessorActivateFailed::what() const throw( )
        {
            return "Activation of RequestProcessor failed";
        }

        const char* RequestProcessorCreateFailed::what() const throw( )
        {
            return "Creation of RequestProcessor failed";
        }

        int handlerPrimaryControllerReactor(
            ZMQReactor* reactor,
            ZMQSocket* handlerReceiverSocket,
            void* lpHandler )
        {
            (void)reactor;
            (void)handlerReceiverSocket;
            (void)lpHandler;
            return -1;
        }

        int workerMessagingReactor(
            ZMQReactor* reactor,
            ZMQSocket* handlerReceiverSocket,
            void* lpHandler )
        {
            (void)reactor;
            (void)handlerReceiverSocket;
            try
            {
                return ( (RequestProcessor*)lpHandler )->handleRequest();
            }
            catch ( const std::exception& ex )
            {
                try
                {
                    clusterLogSetAction( "RequestProcessor.workerMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Fatal,
                            "RequestProcessor.workerMessagingReactor: uncaught exception[ "
                            << ex.what() << "]" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "RequestProcessor.workerMessagingReactor" )
                        clusterLogWithSeverity(
                            LHClusterNS::SeverityLevel::Error,
                            "RequestProcessor.workerMessagingReactor: "
                            "uncaught unknown exception" );
                }
                catch ( ... )
                {
                }
                return -1;
            }
        }

        RequestProcessor::RequestProcessor( const Endpoint& _selfMessagingEndpoint,
            const Endpoint& _selfControllerEndpoint,
            std::unique_ptr< IZMQSocketFactory >
            _socketFactory,
            std::unique_ptr< IZMQSocketSenderReceiver >
            _senderReceiver,
            std::unique_ptr< IRequestHandler >
            _requestHandler )
            : selfMessagingEndpoint( _selfMessagingEndpoint )
            , selfControllerEndpoint( _selfControllerEndpoint )
            , selfMessagingSocket( _socketFactory->Create( SocketType::Pair ) )
            , reactor( zloop_new() )
            , selfSecondarySocket( _socketFactory->Create( SocketType::Pair ) )
            , selfPrimarySocket( _socketFactory->Create( SocketType::Pair ) )
            , reactorThread( nullptr )
            , socketFactory( std::move( _socketFactory ) )
            , senderReceiver( std::move( _senderReceiver ) )
            , requestHandler( std::move( _requestHandler ) )
        {
            if ( !( selfMessagingSocket &&
                reactor &&
                selfSecondarySocket &&
                selfPrimarySocket ) )
            {
                throw RequestProcessorCreateFailed();
            }
            // Should be taken care of in socket factory now ...
            // else
            // {
            //    // I have seen issues with sends to this socket blocking
            //    zsock_set_sndtimeo( workerMessagingSocket, 3 );
            // }
        }

        RequestProcessor::~RequestProcessor()
        {
            try
            {
                Stop();

                if ( reactor )
                {
                    zloop_destroy( &reactor );
                }

                if ( selfMessagingSocket )
                {
                    socketFactory->Destroy( &selfMessagingSocket );
                }

                if ( selfPrimarySocket )
                {
                    socketFactory->Destroy( &selfPrimarySocket );
                }

                if ( selfSecondarySocket )
                {
                    socketFactory->Destroy( &selfSecondarySocket );
                }
            }
            catch ( ... )
            {
                try
                {
                    clusterLogSetAction( "RequestProcessor.destructor" );
                    clusterLogWithSeverity(
                        LHClusterNS::SeverityLevel::Fatal,
                        "RequestProcessor.destructor: exception thrown in destructor" );
                }
                catch ( ... )
                {
                }
            }
        }


        const std::vector< RequestType >& RequestProcessor::SupportedRequestTypes() const
        {
            return requestHandler->SupportedRequestTypes();
        }

        int RequestProcessor::activateMessagingSockets()
        {
            int ret = 0;

            if ( !( selfMessagingSocket ) )
                throw RequestProcessorActivateFailed();

            ret = socketFactory->Bind( selfMessagingSocket, selfMessagingEndpoint.path() ) < 0;

            return ret;
        }

        int RequestProcessor::deactivateMessagingSockets()
        {
            int ret = 0;

            ret += socketFactory->UnBind( selfMessagingSocket, selfMessagingEndpoint.path() );

            ret += RefreshSocket(
                *socketFactory,
                &selfMessagingSocket, SocketType::Pair );

            return ret;
        }


        int RequestProcessor::activateControllerSockets()
        {
            int ret = 0;

            if ( !( selfSecondarySocket && selfPrimarySocket ) )
                throw RequestProcessorActivateFailed();

            ret = socketFactory->Bind(
                selfSecondarySocket, selfControllerEndpoint.path() ) < 0;

            ret += socketFactory->Connect(
                selfPrimarySocket, selfControllerEndpoint.path() ) < 0;

            return ret;
        }

        int RequestProcessor::deactivateControllerSockets()
        {
            int ret = 0;

            ret = socketFactory->DisConnect( selfPrimarySocket, selfControllerEndpoint.path() );

            ret = socketFactory->UnBind( selfSecondarySocket, selfControllerEndpoint.path() );

            ret = RefreshSocket(
                *socketFactory,
                &selfPrimarySocket, SocketType::Pair );

            ret = RefreshSocket(
                *socketFactory,
                &selfSecondarySocket, SocketType::Pair );

            return ret;
        }

        int RequestProcessor::handleRequest( ZMQMessage** lpRequestMsg )
        {
            int ret = 0;
            IRequestHandler::ProcessorResult processorResponse;
            ZMQMessage* msg = *lpRequestMsg;
            ZMQFrame* vfsFrame = nullptr;
            ZMQFrame* requestTypeFrame = nullptr;
            ZMQFrame* requestIdFrame = nullptr;
            ZMQFrameHandler frameHandler;

            clusterLogSetAction( "RequestProcessor.handleRequest" )
                clusterLog( "RequestProcessor.handleRequest: begin2" )

                // 0) remove the version and flags from the message
                vfsFrame = zmsg_pop( msg );
            if ( !( vfsFrame && zframe_size( vfsFrame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.handleRequest: "
                    "failed to parse version and flags" );
                if ( vfsFrame )
                    zframe_destroy( &vfsFrame );
                zmsg_destroy( lpRequestMsg );
                return 1;
            }

            // 1) remove the type from the message
            requestTypeFrame = zmsg_pop( msg );
            if ( !( requestTypeFrame && zframe_size( requestTypeFrame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.handleRequest: "
                    "failed to parse request type" );
                zframe_destroy( &vfsFrame );
                if ( requestTypeFrame )
                    zframe_destroy( &requestTypeFrame );
                zmsg_destroy( lpRequestMsg );
                return 1;
            }

            // 2) remove the id from the message
            requestIdFrame = zmsg_pop( msg );
            if ( !( requestIdFrame && zframe_size( requestIdFrame ) ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.handleRequest: "
                    "failed to parse request id" );
                zframe_destroy( &vfsFrame );
                zframe_destroy( &requestTypeFrame );
                if ( requestIdFrame )
                    zframe_destroy( &requestIdFrame );
                zmsg_destroy( lpRequestMsg );
                return 2;
            }

            // 3) process the request type and request id and the
            //    remainder of the message
            // TODO - processorResponse will leak if something happens
            //        between now and Send
            // TODO - things will crash if the impl does not actually provide a message
            clusterLog( "RequestProcessor.handleRequest: processing" )
                processorResponse = requestHandler->Process(
                    frameHandler.get_frame_value< LHCVersionFlags >( vfsFrame ),
                    frameHandler.get_frame_value< RequestType >( requestTypeFrame ),
                    frameHandler.get_frame_value< RequestId >( requestIdFrame ),
                    msg );
            clusterLog( "RequestProcessor.handleRequest: processed" )

                zmsg_destroy( lpRequestMsg );   //no longer need the request message

            // 4) create the response message with
            //    vfs | type | id | status | rprocessorResponse.second
            frameHandler.prepend_message_value< RequestStatus >(
                processorResponse.second, processorResponse.first );
            zmsg_prepend( processorResponse.second, &requestIdFrame );
            zmsg_prepend( processorResponse.second, &requestTypeFrame );
            zmsg_prepend( processorResponse.second, &vfsFrame );

            clusterLog( "RequestProcessor.handleRequest: sending" )
                ret = senderReceiver->Send(
                    &processorResponse.second,
                    selfMessagingSocket );
            if ( ret != 0 )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.handleRequest: "
                    "failed to send response" );
                zmsg_destroy( &processorResponse.second );
                // TODO - buffer in some container and retry send on timer
            }

            clusterLog( "RequestProcessor.handleRequest2: end" )

                return ret;
        }

        int RequestProcessor::handleRequest()
        {
            int ret = 0;
            ZMQMessage* msg = nullptr;

            clusterLogSetAction( "RequestProcessor.handleRequest" );
            clusterLog( "RequestProcessor.handleRequest: begin1" );

            msg = senderReceiver->Receive( selfMessagingSocket );
            if ( !( msg ) )
            {
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.handleRequest: "
                    "failed to receive message from worker" );
                return 1;
            }

            ret = handleRequest( &msg );
            ret = ret < 0 ? ret : 0;

            clusterLog( "RequestProcessor.handleRequest1: end" );

            return ret;
        }


        void RequestProcessor::Start()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "RequestProcessor.start" )
                clusterLog( "RequestProcessor.start: begin" )

                ret = activateControllerSockets();
            checkRet( ret, 0,
                "RequestProcessor.activateControllerSockets: failed["
                << ret << "]" );

            ret = setupControllerCallbacks();
            checkRet( ret, 0,
                "RequestProcessor.setupControllerCallbacks: failed["
                << ret << "]" );

            reactorThread = std::unique_ptr< std::thread >(
                new std::thread(
                    &RequestProcessor::StartAsPrimary,
                    std::ref( *this ) ) );
            if ( !( reactorThread ) )
            {
                teardownControllerCallbacks();
                deactivateControllerSockets();
                clusterLogWithSeverity(
                    LHClusterNS::SeverityLevel::Error,
                    "RequestProcessor.start: "
                    "failed to create reactor in secondary thread!" );
            }
        }

        void RequestProcessor::Stop()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )
                clusterLogSetAction( "RequestProcessor.stop" )
                clusterLog( "RequestProcessor.stop: begin" );

            if ( reactorThread )
            {
                senderReceiver->Signal( selfPrimarySocket, 0 );

                if ( reactorThread->joinable() )
                {
                    clusterLog( "RequestProcessor.stop: thread joining" );
                    reactorThread->join();
                    clusterLog( "RequestProcessor.stop: thread closed" );
                }
#ifdef DEBUG
                else
                {
                    clusterLog( "RequestProcessor.stop: not joinable" );
                }
#endif
                reactorThread = nullptr;
            }

            ret = teardownControllerCallbacks();
            checkRet( ret, 0,
                "RequestProcessor.teardownControllerCallbacks: failed["
                << ret << "]" );

            ret = deactivateControllerSockets();
            checkRet( ret, 0,
                "RequestProcessor.deactivateControllerSockets: failed["
                << ret << "]" );

            clusterLog( "RequestProcessor.stop: end" );
        }

        void RequestProcessor::StartAsPrimary()
        {
            int ret = 0;

            clusterLogSetActor( selfControllerEndpoint.path() )

                ret = activateMessagingSockets();
            checkRet( ret, 0,
                "RequestProcessor.activateMessagingSockets: failed["
                << ret << "]" );

            ret = setupMessagingCallbacks();
            checkRet( ret, 0,
                "RequestProcessor.setupMessagingCallbacks: failed["
                << ret << "]" );

            ret = zloop_start( reactor );
            clusterLog( "RequestProcessor.startAsPrimary: exiting reactor loop ["
                << errno << "|" << zmq_strerror( errno ) << "]" );

            ret = teardownMessagingCallbacks();
            checkRet( ret, 0,
                "RequestProcessor.teardownMessagingCallbacks: failed["
                << ret << "]" );

            ret = deactivateMessagingSockets();
            checkRet( ret, 0,
                "RequestProcessor.deactivateMessagingSockets: failed["
                << ret << "]" );
        }

        int RequestProcessor::setupControllerCallbacks()
        {
            int ret = 0;

            ret = zloop_reader(
                reactor,
                selfSecondarySocket,
                handlerPrimaryControllerReactor,
                this );

            return ret;
        }

        int RequestProcessor::setupMessagingCallbacks()
        {
            int ret = 0;

            ret = zloop_reader(
                reactor,
                selfMessagingSocket,
                workerMessagingReactor,
                this );

            return ret;
        }

        int RequestProcessor::teardownControllerCallbacks()
        {
            int ret = 0;

            zloop_reader_end(
                reactor,
                selfSecondarySocket );

            return ret;
        }

        int RequestProcessor::teardownMessagingCallbacks()
        {
            int ret = 0;

            zloop_reader_end(
                reactor,
                selfMessagingSocket );

            return ret;
        }
    }
}
