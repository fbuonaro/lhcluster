#ifndef __LHCLUSTER_TEST_H__
#define __LHCLUSTER_TEST_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/worker.h>
#include <lhcluster_impl/zmqframehandler.h>

#ifdef STATS
#define STATS_ASSERT_EQ( expected, stat ) \
    ASSERT_EQ( expected, stat );
#define STATS_ASSERT_EQ2( expected, stat, msg ) \
    ASSERT_EQ( expected, stat ) << msg;
#else
#define STATS_ASSERT_EQ( expected, stat )
#define STATS_ASSERT_EQ2( expected, stat, msg )
#endif

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    void populateFromDataFrames(
        ZMQFrameHandler& frameHandler,
        ZMQMessage* zmqMessage,
        const std::vector< const char* >& data );

    // For Worker + Broker
    void checkReceipt(
        ZMQFrameHandler& frameHandler,
        ZMQMessage* forwardedMsg,
        RequestType requestType,
        RequestId requestId,
        const vector< const char* >& dataFrames,
        bool fromEnv = false );

    // For Worker
    ZMQMessage* createRequestHandlerResponse(
        ZMQFrameHandler& frameHandler,
        RequestType* requestType,
        RequestId* requestId,
        RequestStatus* requestStatus,
        const vector< const char* >& data,
        LHCVersionFlags* vfs );

    // For worker
    ZMQMessage* createBrokerNewRequest(
        ZMQFrameHandler& frameHandler,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage,
        const vector< const char* >& clientEnvelope,
        bool includeNullDelimiter2,
        RequestType* requestType,
        RequestId* requestId,
        const LHCVersionFlags* vfs,
        const vector< const char* >& dataFrames );

    vector< ZMQFrame* > zmqFrameVectorCopy(
        const vector< ZMQFrame* >& zmqFrameVector );

    // For Worker
    void addClientRequestContext(
        Worker& worker,
        RequestType requestType,
        RequestId requestId,
        const LHCVersionFlags& vfs,
        vector< ZMQFrame* >&& envelope );

    // For Broker
    ZMQMessage* createHandlerRequestCompleted( void );
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
                                              LHCVersionFlags* vfs );
    ZMQMessage* createWorkerRequestReceipt(
        ZMQFrameHandler& frameHandler,
        RequestType requestType,
        RequestId requestId,
        const vector< const char* >& env,
        const char* identity );
    ZMQMessage* createWorkerHeartbeat( ZMQFrameHandler& frameHandler,
                                       const char* identity,
                                       bool includeNullDelimiter,
                                       std::time_t* sentAt,
                                       Delay* newHeartbeatSendDelay);
    ZMQMessage* createWorkerMessage( ZMQFrameHandler& frameHandler,
                                     const char* identity,
                                     bool includeNullDelimiter,
                                     ClusterMessage* messageType );
    ZMQMessage* createWorkerReady( ZMQFrameHandler& frameHandler,
                                   const char* identity,
                                   bool includeNullDelimiter,
                                   ClusterMessage* messageType,
                                   LHCVersionFlags* vfs,
                                   int* freeCapacity,
                                   Delay* heartbeatSendDelay,
                                   bool badRequestType,
                                   const vector< RequestType >& supportedRequestTypes );
    ZMQMessage* createClientNewRequest(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& env,
        bool includeNullDelimiter,
        const LHCVersionFlags* responseType,
        RequestType* requestType, 
        RequestId* requestId,
        const vector< const char* >& data,
        bool includeClientMessage = false );

    void checkForwardedClientRequest(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* msg,
        const vector< const char* >& wenv,
        const vector< const char* >& cenv,
        const LHCVersionFlags& vfs,
        RequestType requestType, 
        RequestId requestId,
        const vector< const char* >& data );

    void checkForwardedWorkerResponse(
        unsigned int line,
        ZMQFrameHandler& frameHandler,
        ZMQMessage* msg,
        const vector< const char* >& cenv,
        RequestType requestType, 
        RequestId requestId,
        const vector< const char* >& data,
        const LHCVersionFlags& vfs );

    ZMQMessage* createClientHeartbeat(
        ZMQFrameHandler frameHandler,
        const vector< const char* >& env );

    // For Request Handler
    ZMQMessage* createRequestMessage(
        ZMQFrameHandler& frameHandler,
        LHCVersionFlags* vfs,
        RequestType* requestType,
        RequestId* requestId );

    ZMQMessage* createBrokerMessage(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& identities,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage );

    ZMQMessage* createBrokerHeartbeat(
        ZMQFrameHandler& frameHandler,
        const vector< const char* >& identities,
        bool includeNullDelimiter,
        ClusterMessage* brokerMessage,
        std::time_t* sentAt,
        Delay* newSendDelay );

    class DummyRequestHandler : public IRequestHandler
    {
        public:
            unsigned int numRequestsProcessed;
            vector< ProcessorResult > processRetValues;
            vector< RequestType > supportedRequestTypes;
            vector< tuple< RequestType, RequestId, ZMQMessage*, LHCVersionFlags > >
            processedMessages;

            DummyRequestHandler();
            ~DummyRequestHandler();

            ProcessorResult Process(
                const LHCVersionFlags& vfs,
                RequestType requestType,
                RequestId requestId,
                ZMQMessage* zmqMessage );

            const vector< RequestType >& SupportedRequestTypes() const
            {
                return supportedRequestTypes;
            }
    };
}

#endif
