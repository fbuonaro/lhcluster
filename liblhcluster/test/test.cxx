#include <lhcluster_impl/client.h>
#include <lhcluster_impl/clientproxy.h>
#include <lhcluster_impl/worker.h>

#include "test.h"

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    // For Worker + Broker
    void checkReceipt(
        ZMQFrameHandler& frameHandler,
        ZMQMessage* forwardedMsg,
        RequestType requestType,
        RequestId requestId,
        const vector< const char* >& dataFrames,
        bool fromEnv )
    {
        vector< const char* > empty;
        ZMQFrame* frame = nullptr;
        RequestType msgRequestType;
        RequestId msgRequestId;
        ClusterMessage bm;
        ClusterMessage wm;
        int modifier = 0;

        // null | wm::receipt | env | bm:receipt | rt | rid
        if( !fromEnv )
        {
            frame = zmsg_first( forwardedMsg );
            ASSERT_TRUE( frame != nullptr );
            ASSERT_TRUE( zframe_size( frame ) == 0 );

            frame = zmsg_next( forwardedMsg );
            ASSERT_TRUE( frame != nullptr );
            ASSERT_TRUE( zframe_size( frame ) > 0 );
            wm = frameHandler.get_frame_value< ClusterMessage >( frame );
            ASSERT_EQ( ClusterMessage::WorkerReceipt, wm );
        }
        else
        {
            string data;
            auto it = dataFrames.begin();
            frame = zmsg_first( forwardedMsg );
            ASSERT_TRUE( frame != nullptr );
            ASSERT_TRUE( zframe_size( frame ) > 0 );
            data = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), data ) << *it << " != " << data.c_str();
            modifier = 1;
        }

        for( auto it = dataFrames.begin() + modifier; it != dataFrames.end(); ++it )
        {
            string data;
            frame = zmsg_next( forwardedMsg );
            ASSERT_TRUE( frame != nullptr );
            ASSERT_TRUE( zframe_size( frame ) > 0 );
            data = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), data ) << *it << " != " << data.c_str();
        }

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) == 0 );

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        bm = frameHandler.get_frame_value< ClusterMessage >( frame );
        ASSERT_EQ( ClusterMessage::BrokerReceivedReceipt, bm );

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        msgRequestType = frameHandler.get_frame_value< RequestType >( frame );
        ASSERT_EQ( requestType, msgRequestType );


        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame != nullptr );
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        msgRequestId = frameHandler.get_frame_value< RequestId >( frame );
        ASSERT_EQ( requestId, msgRequestId );

        frame = zmsg_next( forwardedMsg );
        ASSERT_TRUE( frame == nullptr );
    }

    // For Worker
    ZMQMessage* createRequestHandlerResponse(
        ZMQFrameHandler& frameHandler,
        RequestType* requestType,
        RequestId* requestId,
        RequestStatus* requestStatus,
        const vector< const char* >& dataFrames,
        LHCVersionFlags* vfs )
    {
        ZMQMessage* msg = zmsg_new();

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }
            
        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestStatus )
            frameHandler.append_message_value( msg, *requestStatus );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        for( auto it = dataFrames.begin(); it != dataFrames.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        return msg;
    }

    // For Worker
    ZMQMessage* createBrokerNewRequest(
        ZMQFrameHandler& frameHandler,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage,
        const vector< const char* >& clientEnvelope,
        bool includeNullDelimiter2,
        RequestType* requestType,
        RequestId* requestId,
        const LHCVersionFlags* vfs,
        const vector< const char* >& dataFrames )
    {
        vector< const char* > empty;
        ZMQMessage* msg =
            createBrokerMessage(
                frameHandler, empty, includeNullDelimiter, brokerMessage );

        for( auto it = clientEnvelope.begin(); it != clientEnvelope.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        if( includeNullDelimiter2 )
            zmsg_addmem( msg, nullptr, 0 );

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        for( auto it = dataFrames.begin(); it != dataFrames.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        return msg;
    }


    // for Broker
    vector< ZMQFrame* > zmqFrameVectorCopy(
        const vector< ZMQFrame* >& zmqFrameVector )
    {
        vector< ZMQFrame* > ret;
        for( auto it = zmqFrameVector.begin();
             it != zmqFrameVector.end();
             ++it )
             ret.push_back( zframe_dup( *it ) );
         return ret;
    }

    void addClientRequestContext(
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        vector< ZMQFrame* >&& envelope )
    {
        ClientRequestContext newRequestContext(
            requestType,
            requestId,
            vfs,
            forward< vector< ZMQFrame* > >( envelope ) );
        worker.requestContexts.insert(
            std::pair<
                std::pair< RequestType, RequestId >,
                ClientRequestContext&& >( 
                    std::pair< RequestType, RequestId>(
                        newRequestContext.GetRequestType(),
                        newRequestContext.GetRequestId() ),
                    std::move( newRequestContext ) ) );
    }

    void populateFromDataFrames(
        ZMQFrameHandler& frameHandler,
        ZMQMessage* zmqMessage,
        const std::vector< const char* >& data )
    {
        for( auto it = data.begin(); it != data.end(); ++it )
            frameHandler.append_message_value( zmqMessage, string( *it ) );
    }

    ZMQMessage* createWorkerRequestCompleted( ZMQFrameHandler& frameHandler,
                                              const char* identity,
                                              bool includeNullDelimiter,
                                              ClusterMessage* workerMessage,
                                              const vector< const char* >& clientEnvelope,
                                              bool includeNullDelimiter2,
                                              ClusterMessage* brokerMessage,
                                              RequestType* requestType,
                                              RequestId* requestId,
                                              const vector< const char* >& data,
                                              LHCVersionFlags* vfs )
    {
        ZMQMessage* msg = createWorkerMessage( frameHandler, identity, includeNullDelimiter,
                                               workerMessage );

        for( auto it = clientEnvelope.begin(); it != clientEnvelope.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        if( includeNullDelimiter2 )
            zmsg_addmem( msg, nullptr, 0 );

        if( brokerMessage )
            frameHandler.append_message_value( msg, *brokerMessage );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        for( auto it = data.begin(); it != data.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        return msg;
    }

    ZMQMessage* createWorkerRequestReceipt(
        ZMQFrameHandler& frameHandler,
        RequestType requestType,
        RequestId requestId,
        const vector< const char* >& env,
        const char* identity )
    {
        ZMQMessage* msg = zmsg_new();

        frameHandler.append_message_value( msg, string( identity ) );
        zmsg_addmem( msg, nullptr, 0 );
        frameHandler.append_message_value( msg, ClusterMessage::WorkerReceipt );
        for( auto it = env.begin(); it != env.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );
        zmsg_addmem( msg, nullptr, 0 );
        frameHandler.append_message_value( msg, ClusterMessage::BrokerReceivedReceipt );
        frameHandler.append_message_value( msg, requestType );
        frameHandler.append_message_value( msg, requestId );

        return msg;
    }

    ZMQMessage* createWorkerHeartbeat( ZMQFrameHandler& frameHandler,
                                       const char* identity,
                                       bool includeNullDelimiter,
                                       std::time_t* sentAt,
                                       Delay* newHeartbeatSendDelay)
    {
        ClusterMessage workerMessage( ClusterMessage::WorkerHeartbeat );
        ZMQMessage* msg = createWorkerMessage(
                            frameHandler,
                            identity,
                            includeNullDelimiter,
                            &workerMessage );
        if( sentAt )
            frameHandler.append_message_value( msg, *sentAt );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( newHeartbeatSendDelay )
            frameHandler.append_message_value( msg, newHeartbeatSendDelay->count() );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        return msg;
    }

    ZMQMessage* createClientNewRequest(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& env,
        bool includeNullDelimiter,
        const LHCVersionFlags* vfs,
        RequestType* requestType, 
        RequestId* requestId,
        const vector< const char* >& data,
        bool includeClientMessage )
    {
        ZMQMessage* msg = zmsg_new();

        for( auto it = env.begin(); it != env.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        if( includeNullDelimiter )
            zmsg_addmem( msg, NULL, 0 );

        if( includeClientMessage )
            frameHandler.append_message_value( msg, ClusterMessage::ClientNewRequest );

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestType )
            frameHandler.append_message_value( msg, *requestType );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( requestId )
            frameHandler.append_message_value( msg, *requestId );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        for( auto it = data.begin(); it != data.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        return msg;
    }

    void checkForwardedWorkerResponse(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* msg,
        const vector< const char* >& cenv,
        RequestType requestType, 
        RequestId requestId,
        const vector< const char* >& data,
        const LHCVersionFlags& vfs )
    {
        ZMQFrame* frame = nullptr;
        // expecting the request sent from the broker to be formatted something like
        unsigned int expectedNumFrames =
            cenv.size() + 1     + 1              + 3         + data.size();
        //  client env  | null  | broker message | vfs|rt|rid| data
        ASSERT_EQ( expectedNumFrames, zmsg_size( msg ) ) << "insufficient number of frames " << line;

        frame = zmsg_first( msg );
        ASSERT_TRUE( frame != nullptr );
        for( auto it = cenv.begin(); it != cenv.end(); ++it )
        {
            string frameStr = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), frameStr ) << "failure at line " << line;
            frame = zmsg_next( msg );
        }

        ASSERT_EQ( 0, zframe_size( frame ) ) << "failure at line " << line;

        frame = zmsg_next( msg );
        ASSERT_EQ( ClusterMessage::BrokerResponse,
                   frameHandler.get_frame_value< ClusterMessage >( frame ) ) << "failure at line "
                                                                            << line; 
        frame = zmsg_next( msg );
        ASSERT_EQ( vfs, frameHandler.get_frame_value< LHCVersionFlags >( frame ) );
        frame = zmsg_next( msg );
        ASSERT_EQ( requestType, frameHandler.get_frame_value< RequestType >( frame ) );
        frame = zmsg_next( msg );
        ASSERT_EQ( requestId, frameHandler.get_frame_value< RequestId >( frame ) );

        frame = zmsg_next( msg );
        for( auto it = data.begin(); it != data.end(); ++it )
        {
            string frameStr = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), frameStr ) << "failure at line " << line;
            frame = zmsg_next( msg );
        }

        ASSERT_EQ( nullptr, frame );
    }

    void checkForwardedClientRequest(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* msg,
        const vector< const char* >& wenv,
        const vector< const char* >& cenv,
        const LHCVersionFlags& vfs,
        RequestType requestType, 
        RequestId requestId,
        const vector< const char* >& data )
    {
        ZMQFrame* frame = nullptr;
        // expecting the request sent from the broker to be formatted something like
        unsigned int expectedNumFrames =
            wenv.size() + 1    + 1              + cenv.size()   + 1    + 3          + data.size();
        //  worker env  | null | broker message | client env    | null | vfs|rt|rid | data
        ASSERT_EQ( expectedNumFrames, zmsg_size( msg ) )
            << "insufficient number of frames " << line;

        frame = zmsg_first( msg );
        ASSERT_TRUE( frame != nullptr );
        for( auto it = wenv.begin(); it != wenv.end(); ++it )
        {
            string frameStr = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), frameStr ) << "failure at line " << line;
            frame = zmsg_next( msg );
        }

        ASSERT_EQ( 0, zframe_size( frame ) ) << "failure at line " << line;

        frame = zmsg_next( msg );
        ASSERT_EQ( ClusterMessage::BrokerNewRequest,
                   frameHandler.get_frame_value< ClusterMessage >( frame ) ) << "failure at line "
                                                                            << line; 
        frame = zmsg_next( msg );
        for( auto it = cenv.begin(); it != cenv.end(); ++it )
        {
            string frameStr = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), frameStr ) << "failure at line " << line;
            frame = zmsg_next( msg );
        }
        
        ASSERT_EQ( 0, zframe_size( frame ) ) << "failure at line " << line;

        frame = zmsg_next( msg );
        ASSERT_EQ( vfs, frameHandler.get_frame_value< LHCVersionFlags >( frame ) );
        frame = zmsg_next( msg );
        ASSERT_EQ( requestType, frameHandler.get_frame_value< RequestType >( frame ) );
        frame = zmsg_next( msg );
        ASSERT_EQ( requestId, frameHandler.get_frame_value< RequestId >( frame ) );

        // skip
        frame = zmsg_next( msg );

        for( auto it = data.begin(); it != data.end(); ++it )
        {
            string frameStr = frameHandler.get_frame_value< string >( frame );
            ASSERT_EQ( string( *it ), frameStr ) << "failure at line " << line;
            frame = zmsg_next( msg );
        }

        ASSERT_EQ( nullptr, frame );
    }


    ZMQMessage* createClientHeartbeat(
        ZMQFrameHandler frameHandler,
        const vector< const char* >& env )
    {
        ZMQMessage* msg = zmsg_new();

        for( auto it = env.begin(); it != env.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );
        zmsg_addmem( msg, nullptr, 0 );
        frameHandler.append_message_value( msg, ClusterMessage::ClientHeartbeat );

        return msg;
    }

    // for Broker
    ZMQMessage* createWorkerMessage( ZMQFrameHandler& frameHandler,
                                     const char* identity,
                                     bool includeNullDelimiter,
                                     ClusterMessage* messageType )
    {
        ZMQMessage* msg = zmsg_new();

        if( identity )
            frameHandler.append_message_value( msg, string( identity ) );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( includeNullDelimiter )
            zmsg_addmem( msg, nullptr, 0 );

        
        if( messageType )
            frameHandler.append_message_value< ClusterMessage >( msg, *messageType );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        return msg;
    }

    ZMQMessage* createWorkerReady( ZMQFrameHandler& frameHandler,
                                   const char* identity,
                                   bool includeNullDelimiter,
                                   ClusterMessage* messageType,
                                   LHCVersionFlags* vfs,
                                   int* freeCapacity,
                                   Delay* heartbeatSendDelay,
                                   bool badRequestType,
                                   const vector< RequestType >& supportedRequestTypes )
    {
        ZMQMessage* msg = createWorkerMessage(
            frameHandler, identity, includeNullDelimiter, messageType );

        if( vfs )
            frameHandler.append_message_value( msg, *vfs );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( freeCapacity )
            frameHandler.append_message_value( msg, *freeCapacity );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( heartbeatSendDelay )
            frameHandler.append_message_value< int >(
                msg,
                heartbeatSendDelay->count() );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        if( badRequestType )
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }
        else
        {
            for( auto it = supportedRequestTypes.rbegin();
                 it != supportedRequestTypes.rend();
                 ++it )
            {
                frameHandler.append_message_value( msg, *it );
            }
        }

        return msg;
    }

    // For request handler
    // Message must be RequestType | RequestId [ | More ]*
    ZMQMessage* createRequestMessage(
        ZMQFrameHandler& frameHandler,
        LHCVersionFlags* vfs,
        RequestType* requestType,
        RequestId* requestId )
    {
        ZMQMessage* zmqMessage = zmsg_new();

        if( vfs )
            frameHandler.append_message_value( zmqMessage, *vfs );
        else
        {
            zmsg_addmem( zmqMessage, nullptr, 0 );
            return zmqMessage;
        }

        if( requestType )
            frameHandler.append_message_value( zmqMessage, *requestType );
        else
        {
            zmsg_addmem( zmqMessage, nullptr, 0 );
            return zmqMessage;
        }


        if( requestId )
            frameHandler.append_message_value( zmqMessage, *requestId );
        else
        {
            zmsg_addmem( zmqMessage, nullptr, 0 );
            return zmqMessage;
        }

        return zmqMessage;
    }

    // A broker message consists of
    //  1) one or more nonempty frames entailinig the broker identity
    //  2) one null delimiter
    //  2) one broker message type (heartbeat, response, receipt)
    ZMQMessage* createBrokerMessage(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& identities,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage
    )
    {
        ZMQMessage* msg = zmsg_new();

        for( auto it = identities.begin(); it != identities.end(); ++it )
            frameHandler.append_message_value( msg, string( *it ) );

        if( includeNullDelimiter )
            zmsg_addmem( msg, nullptr, 0 );

        if( brokerMessage )
            frameHandler.append_message_value( msg, *brokerMessage );
        else
        {
            zmsg_addmem( msg, nullptr, 0 );
            return msg;
        }

        return msg;
    }

    //  a) If Heartbeat then
    //  3a) one delimiter
    //  4a) timestamp for sent at
    //  5a) timestamp for new send delay
    ZMQMessage* createBrokerHeartbeat(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& identities,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage,
        std::time_t* sentAt,
        Delay* newSendDelay )
    {
        ZMQMessage* msg = createBrokerMessage(
            frameHandler,
            identities,
            includeNullDelimiter,
            brokerMessage );
        
        if( sentAt )
            frameHandler.append_message_value( msg, *sentAt );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        if( newSendDelay )
            frameHandler.append_message_value< int >(
                msg,
                newSendDelay->count() );
        else
        {
            zmsg_addmem( msg, NULL, 0 );
            return msg;
        }

        return msg;
    }

    IRequestHandler::ProcessorResult DummyRequestHandler::Process(
        const LHCVersionFlags& vfs,
        RequestType requestType,
        RequestId requestId,
        ZMQMessage* zmqMessage )
    {
        IRequestHandler::ProcessorResult ret;
        if( numRequestsProcessed >= processRetValues.size() )
            ret = processRetValues.back();
        else
            ret = processRetValues[ numRequestsProcessed ];
        
        ++numRequestsProcessed;

        processedMessages.emplace_back(
            requestType,
            requestId,
            zmsg_dup( zmqMessage ),
            vfs );

        return IRequestHandler::ProcessorResult(
            ret.first,
            zmsg_dup( ret.second ) );
    }
            
    DummyRequestHandler::~DummyRequestHandler()
    {
        for( auto it = processedMessages.begin(); 
                it != processedMessages.end();
                ++it )
        {
            ZMQMessage* zmqMessage = std::get<2>( *it );
            zmsg_destroy( &zmqMessage );
        }

        for( auto it = processRetValues.begin(); 
                it != processRetValues.end();
                ++it )
        {
            ZMQMessage* zmqMessage = it->second;
            zmsg_destroy( &zmqMessage );
        }

    }

    DummyRequestHandler::DummyRequestHandler()
    :   IRequestHandler()
    ,   numRequestsProcessed( 0 )
    ,   processRetValues()
    ,   supportedRequestTypes()
    {
        processRetValues.emplace_back( RequestStatus::Succeeded, nullptr );
    }
}
