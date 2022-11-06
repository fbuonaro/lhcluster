#include <lhcluster/brokerfactory.h>
#include <lhcluster/clientfactory.h>
#include <lhcluster/clientproxyfactory.h>
#include <lhcluster/workerfactory.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <chrono>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace std;

    class TestBrokerEndToEnd : public ::testing::Test
    {
    protected:

        void SetUp()
        {
        }

        void Tear()
        {
        }

        BrokerParameters getBrokerParameters()
        {
            BrokerParameters brokerParams;

            brokerParams.SetBackendMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_bbe.zitc" ) );

            brokerParams.SetControlEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_bce.zitc" ) );

            brokerParams.SetFrontendMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_bfe.zitc" ) );

            // it is supposed to be the delay between heartbeats sent to the broker
            // this number is communicated to workers, clientproxies in the heartbeat
            // message that the broker sends to them
            // However, it is not really supported by either of those actors yet and is
            // simply ignored and does not alter the frequency of heartbeats
            // the original intent was that it would be possible to adjust the heartbeat
            // frequency at runtime if under heavy load/adapt to other networking conditions
            //
            // AS IS its only effect is that workers are considered "dead" if the broker
            // has not received a heartbeat from them in 2 * selfHeartbeatDelay seconds
            // i.e. at a self heartbeat delay of 3 seconds, if a worker does not send a
            // heartbeat for seconds then it will be considered dead after 6 seconds
            brokerParams.SetSelfHeartbeatDelay( Delay( 6 ) );

            // the minimum amount of time between heartbeats sent by the broker to the
            // workers, the broker will check for heartbeats to send
            // every minimumWorkerHeartbeatDelay seconds
            // the workers themselves can specify that heartbeats be sent at a larger
            // interval however
            // the heartbeats will always be sent every
            // brokerMinimum * ceil( workerSpecified / brokerMinimum ) seconds
            brokerParams.SetMinimumWorkerHeartbeatDelay( Delay( 6 ) );

            // the amount of time between when the broker will check if any workers are dead
            brokerParams.SetWorkerPulseDelay( Delay( 3 ) );

            return brokerParams;
        }

        ClientParameters getClientParameters(
            const ClientProxyParameters& clientProxyParameters )
        {
            ClientParameters clientParameters;

            // endpoint of this client
            clientParameters.SetClientMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_cm.zitc" ) );

            // endpoint of the clientproxy's client facing endpoint
            clientParameters.SetClientProxyMessagingEndpoint(
                clientProxyParameters.GetSelfClientMessagingEndpoint() );

            return clientParameters;
        }

        ClientProxyParameters getClientProxyParameters(
            const BrokerParameters& brokerParams )
        {
            ClientProxyParameters clientProxyParameters;

            clientProxyParameters.SetSelfBrokerMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_cpbm.zitc" ) );

            clientProxyParameters.SetBrokerMessagingEndpoint(
                brokerParams.GetFrontendMessagingEndpoint() );

            clientProxyParameters.SetSelfControllerEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_cpce.zitc" ) );

            clientProxyParameters.SetSelfClientMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_cpcm.zitc" ) );

            // the broker will only send a heartbeat to the clientproxy in response
            // to a clientproxy heartbeat
            // this delay represents the maximum amount of time the broker should take
            // to send the heartbeat to the clientproxy
            // AS IS if a broker heartbeat is not received within 2 * selfHeartbeatDelay
            // then the clientproxy will consider the broker as dead
            clientProxyParameters.SetSelfHeartbeatDelay( Delay( 6 ) );

            // the delay between sending heartbeats to the broker
            clientProxyParameters.SetInitialBrokerHeartbeatDelay( Delay( 6 ) );

            // the delay between checking whether the broker is still alive
            clientProxyParameters.SetBrokerPulseDelay( Delay( 3 ) );

            return clientProxyParameters;
        }

        WorkerParameters getWorkerParameters(
            const BrokerParameters& brokerParams )
        {
            WorkerParameters workerParameters;

            workerParameters.SetSelfMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_wsm.zitc" ) );

            workerParameters.SetBrokerMessagingEndpoint(
                brokerParams.GetBackendMessagingEndpoint() );

            workerParameters.SetSelfControllerEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_wce.zitc" ) );

            workerParameters.SetSelfInternalEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_wie.zitc" ) );

            workerParameters.SetRequestHandlerInternalEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_rhie.zitc" ) );

            workerParameters.SetRequestHandlerControllerEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_rhce.zitc" ) );

            // this delay represents the maximum amount of time the broker should take
            // to send the heartbeat to the worker
            // AS IS if a broker heartbeat is not received within 2 * selfHeartbeatDelay
            // then the worker will consider the broker as dead
            workerParameters.SetSelfHeartbeatDelay( Delay( 6 ) );

            // the delay between sending heartbeats to the broker
            workerParameters.SetInitialBrokerHeartbeatDelay( Delay( 6 ) );

            // the delay between checking whether the broker is still alive
            workerParameters.SetBrokerPulseDelay( Delay( 3 ) );

            return workerParameters;
        }
    };

    class TestRequestHandlerEndToEnd1 : public IRequestHandler
    {
    public:
        unordered_map< RequestType, vector< pair< RequestId, string > > > requests;
        unordered_map< RequestType, int > expectedRequests;
        int extraMessagesReceived;
        int badMessagesReceived;
        vector< RequestType > supportedRequestTypes;

        TestRequestHandlerEndToEnd1()
            : IRequestHandler()
            , requests()
            , expectedRequests()
            , extraMessagesReceived( 0 )
            , badMessagesReceived( 0 )
            , supportedRequestTypes( { 1,2,3,4 } )
        {
        }

        ~TestRequestHandlerEndToEnd1()
        {
        }

        const std::vector< RequestType >& SupportedRequestTypes() const
        {
            return supportedRequestTypes;
        }

        ProcessorResult Process(
            const LHCVersionFlags& vfs,
            RequestType requestType,
            RequestId requestId,
            ZMQMessage* msg )
        {
            auto itExpected = expectedRequests.find( requestType );
            int msgSize = zmsg_size( msg );
            RequestStatus requestStatus = RequestStatus::Failed;
            ZMQMessage* response( zmsg_dup( msg ) );

            fprintf( stderr, "RECEIVED[%u|%d]\n", requestType, msgSize );

            if ( itExpected != expectedRequests.end() )
            {
                if ( ( requestId > 0 ) && ( msgSize == 1 ) && ( vfs.flags == 0 ) )
                {
                    Impl::ZMQFrameHandler frameHandler;
                    string messageString =
                        frameHandler.get_frame_value< string >( zmsg_first( msg ) );
                    auto itReceived = requests.find( requestType );

                    if ( itReceived != requests.end() )
                    {
                        int receivedCount = itReceived->second.size();

                        if ( receivedCount < itExpected->second )
                        {
                            itReceived->second.emplace_back( requestId, messageString );

                            if ( receivedCount == ( itExpected->second - 1 ) )
                            {
                                requestStatus = RequestStatus::Succeeded;
                            }
                            else
                            {
                                requestStatus = RequestStatus::InProgress;
                            }
                        }
                        else
                        {
                            ++extraMessagesReceived;
                        }
                    }
                    else
                    {
                        requests.emplace(
                            requestType,
                            vector< pair< RequestId, string > >{
                            pair< RequestId, string >(
                                requestId,
                                messageString ) } );
                        requestStatus = RequestStatus::InProgress;
                    }
                }
                else
                {
                    ++badMessagesReceived;
                }
            }
            else
            {
                if ( ( requestId == 0 ) && ( msgSize == 1 ) && vfs.hasAll( MessageFlag::Asynchronous ) )
                {
                    Impl::ZMQFrameHandler frameHandler;
                    int expectedCount = frameHandler.get_frame_value< int >(
                        zmsg_first( msg ) );
                    expectedRequests.emplace( requestType, expectedCount );
                    requestStatus = RequestStatus::New;
                }
                else
                {
                    ++badMessagesReceived;
                }
            }

            return ProcessorResult( requestStatus, response );
        }
    };

    class TestRequestHandlerEndToEnd1Wrapper : public IRequestHandler
    {
    public:
        TestRequestHandlerEndToEnd1Wrapper( TestRequestHandlerEndToEnd1& toWrap )
            : IRequestHandler()
            , wrapped( toWrap )
        {
        }

        const std::vector< RequestType >& SupportedRequestTypes() const
        {
            return wrapped.SupportedRequestTypes();
        }

        ProcessorResult Process(
            const LHCVersionFlags& vfs,
            RequestType requestType,
            RequestId requestId,
            ZMQMessage* msg )
        {
            return wrapped.Process( vfs, requestType, requestId, msg );
        }


    private:
        TestRequestHandlerEndToEnd1& wrapped;
    };

    TEST_F( TestBrokerEndToEnd, TestEndToEnd1 )
    {
        BrokerParameters brokerParams( getBrokerParameters() );
        ClientProxyParameters clientProxyParameters( getClientProxyParameters( brokerParams ) );
        ClientParameters clientParameters( getClientParameters( clientProxyParameters ) );
        WorkerParameters workerParameters( getWorkerParameters( brokerParams ) );
        Broker broker(
            BrokerFactory().CreateConcrete( brokerParams ) );
        Client client( ClientFactory().CreateConcrete( clientParameters ) );
        ClientProxy clientProxy(
            ClientProxyFactory().CreateConcrete( clientProxyParameters ) );
        TestRequestHandlerEndToEnd1 testHandler;
        Worker worker(
            WorkerFactory().CreateConcrete(
                workerParameters,
                unique_ptr< IRequestHandler >(
                    new TestRequestHandlerEndToEnd1Wrapper( testHandler ) ) ) );

        vector< pair< RequestType, int > > requestCounts{
            { 1, 1 },
            { 2, 2 },
            { 3, 3 },
            { 4, 4 }
        };

        vector< pair< RequestType, pair< RequestId, string > > > requestsToSend{
            { 1, { 1, "first" } },
            { 1, { 2, "extra1" } },
            { 1, { 3, "extra2" } },
            { 2, { 1, "first" } },
            { 2, { 2, "second" } },
            { 2, { 3, "extra1" } },
            { 3, { 1, "first" } },
            { 3, { 2, "second" } },
            { 3, { 3, "third" } },
            { 2, { 4, "extra2" } },
            { 4, { 1, "first" } },
            { 4, { 2, "second" } },
            { 4, { 3, "third" } },
            { 4, { 4, "fourth" } }
        };

        Impl::ZMQFrameHandler frameHandler;
        int ret = 0;

        vector< RequestResponse > responses;

        broker.Start();
        clientProxy.Start();
        worker.Start();
        client.Activate();

        // slight delay to ensure connectivity
        this_thread::sleep_for( chrono::milliseconds( 300 ) );

        for ( auto it = requestCounts.cbegin(); it != requestCounts.cend(); ++it )
        {
            ZMQMessage* countMsg = zmsg_new();

            frameHandler.append_message_value< int >( countMsg, it->second );

            ret = client.SendAsynchronousRequest( it->first, 0, &countMsg );
            EXPECT_EQ( 0, ret ) << "failed to send count[" << it->first << "]";
            if ( ret != 0 )
            {
                zmsg_destroy( &countMsg );
            }

            this_thread::sleep_for( chrono::milliseconds( 80 ) );
        }


        for ( auto it = requestsToSend.cbegin(); it != requestsToSend.cend(); ++it )
        {
            ZMQMessage* msg = zmsg_new();

            frameHandler.append_message_value< string >( msg, it->second.second );

            ret = client.SendSynchronousRequest( it->first, it->second.first, &msg );
            EXPECT_EQ( 0, ret ) << "failed to send msg[" << it->first << "]";
            if ( ret != 0 )
            {
                zmsg_destroy( &msg );
            }
        }

        client.RetrieveResponses( responses, requestsToSend.size(), chrono::milliseconds( 160 ) );

        client.Deactivate();
        worker.Stop();
        clientProxy.Stop();
        broker.Stop();

        EXPECT_EQ( 0, testHandler.badMessagesReceived );
        EXPECT_EQ( requestCounts.size(), testHandler.expectedRequests.size() );
        EXPECT_EQ( requestsToSend.size(), responses.size() );

        for ( auto it = requestCounts.cbegin(); it != requestCounts.cend(); ++it )
        {
            auto itExpected = testHandler.expectedRequests.find( it->first );
            auto itRequest = testHandler.requests.find( it->first );

            EXPECT_NE( itExpected, testHandler.expectedRequests.end() )
                << "request count not received, "
                "possibly a timing issue - try increasing ms delay[" << it->first << "]";
            EXPECT_NE( itRequest, testHandler.requests.end() )
                << "request msgs not received["
                "possibly a timing issue - try increasing ms delay[" << it->first << "]";

            if ( itExpected != testHandler.expectedRequests.end() )
            {
                EXPECT_EQ( it->second, itExpected->second )
                    << "request count not matching[" << it->first << "]";
            }

            if ( itRequest != testHandler.requests.end() )
            {
                EXPECT_TRUE( (size_t)it->second <= itRequest->second.size() )
                    << "request msg counts not matching[" << it->first << "]";
            }
        }
    }
}
