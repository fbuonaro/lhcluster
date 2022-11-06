#include <lhcluster/notifybrokerfactory.h>
#include <lhcluster/notifypublisherfactory.h>
#include <lhcluster/notifysubscriberfactory.h>
#include <lhcluster_impl/zmqframehandler.h>

#include <chrono>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

// #define DIRECT_TO_PUB

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace std;

    class TestNotifyBroker : public ::testing::Test
    {
    protected:

        void SetUp()
        {
        }

        void Tear()
        {
        }

        // pub connects to backend, sub to frontend

        NotifyBrokerParameters getNotifyBrokerParameters()
        {
            NotifyBrokerParameters brokerParams;

            brokerParams.SetFrontendMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_fe.zitc" ) );
            brokerParams.SetBackendMessagingEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_be.zitc" ) );

            return brokerParams;
        }

        NotifyPublisherParameters getNotifyPubParameters(
            const NotifyBrokerParameters& brokerParams )
        {
            NotifyPublisherParameters pubParams;

#ifndef DIRECT_TO_PUB
            pubParams.SetProxyEndpoint( brokerParams.GetBackendMessagingEndpoint() );
#else
            pubParams.SetSelfEndpoint( brokerParams.GetFrontendMessagingEndpoint() );
#endif

            return pubParams;
        }

        NotifySubscriberParameters getNotifySubParameters(
            const NotifyBrokerParameters& brokerParams )
        {
            NotifySubscriberParameters subParams;

            subParams.SetSelfControllerEndpoint(
                Endpoint( EndpointType::InterThread, "testnotifybrokere2e_subcontroller.zitc" ) );
            subParams.SetPublisherEndpoint( brokerParams.GetFrontendMessagingEndpoint() );

            return subParams;
        }
    };

    class TestNotifyHandlerEndToEnd1 : public INotifyHandler
    {
    public:
        unordered_map< NotificationType, vector< pair< RequestId, string > > > notifications;
        unordered_map< NotificationType, int > expectedNotifications;
        int extraMessagesReceived;
        int badMessagesReceived;

        TestNotifyHandlerEndToEnd1()
            : INotifyHandler()
            , notifications()
            , expectedNotifications()
            , extraMessagesReceived( 0 )
            , badMessagesReceived( 0 )
        {
        }

        ~TestNotifyHandlerEndToEnd1()
        {
        }

        RequestStatus Process(
            const NotificationType& notificationType,
            ZMQMessage* notificationMsg )
        {
            auto itExpected = expectedNotifications.find( notificationType );
            int msgSize = zmsg_size( notificationMsg );
            RequestStatus requestStatus = RequestStatus::Failed;

            fprintf( stderr, "RECEIVED[%s|%d]\n", notificationType.c_str(), msgSize );

            if ( itExpected != expectedNotifications.end() )
            {
                if ( msgSize == 2 )
                {
                    Impl::ZMQFrameHandler frameHandler;
                    RequestId requestId =
                        frameHandler.get_frame_value< RequestId >( zmsg_first( notificationMsg ) );
                    string messageString =
                        frameHandler.get_frame_value< string >( zmsg_next( notificationMsg ) );
                    auto itReceived = notifications.find( notificationType );

                    if ( itReceived != notifications.end() )
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
                        notifications.emplace(
                            notificationType,
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
                if ( msgSize == 1 )
                {
                    Impl::ZMQFrameHandler frameHandler;
                    int expectedCount = frameHandler.get_frame_value< int >(
                        zmsg_first( notificationMsg ) );
                    expectedNotifications.emplace( notificationType, expectedCount );
                    requestStatus = RequestStatus::New;
                }
                else
                {
                    ++badMessagesReceived;
                }
            }

            return requestStatus;
        }
    };

    class TestNotifyHandlerEndToEnd1Wrapper : public INotifyHandler
    {
    public:
        TestNotifyHandlerEndToEnd1Wrapper( TestNotifyHandlerEndToEnd1& toWrap )
            : INotifyHandler()
            , wrapped( toWrap )
        {
        }

        RequestStatus Process(
            const NotificationType& notificationType,
            ZMQMessage* notificationMsg )
        {
            return wrapped.Process( notificationType, notificationMsg );
        }

    private:
        TestNotifyHandlerEndToEnd1& wrapped;
    };

    TEST_F( TestNotifyBroker, TestEndToEnd1 )
    {
        NotifyBrokerParameters brokerParams( getNotifyBrokerParameters() );
#ifndef DIRECT_TO_PUB
        NotifyBroker notifyBroker(
            NotifyBrokerFactory().CreateConcrete( brokerParams ) );
#endif
        NotifyPublisher notifyPublisher(
            NotifyPublisherFactory().CreateConcrete( getNotifyPubParameters( brokerParams ) ) );
        TestNotifyHandlerEndToEnd1 testHandler;
        NotifySubscriber notifySubscriber(
            NotifySubscriberFactory().CreateConcrete(
                getNotifySubParameters( brokerParams ),
                unique_ptr< INotifyHandler >(
                    new TestNotifyHandlerEndToEnd1Wrapper( testHandler ) ) ) );

        vector< pair< NotificationType, int > > notificationCounts{
            { "n1", 1 },
            { "n2", 2 },
            { "n3", 3 },
            { "n4", 4 }
        };

        vector< pair< NotificationType, pair< RequestId, string > > > notificationsToSend{
            { "n1", { 1, "first" } },
            { "n1", { 2, "extra1" } },
            { "n1", { 3, "extra2" } },
            { "n2", { 1, "first" } },
            { "n2", { 2, "second" } },
            { "n2", { 3, "extra1" } },
            { "n3", { 1, "first" } },
            { "n3", { 2, "second" } },
            { "n3", { 3, "third" } },
            { "n2", { 4, "extra2" } },
            { "n4", { 1, "first" } },
            { "n4", { 2, "second" } },
            { "n4", { 3, "third" } },
            { "n4", { 4, "fourth" } }
        };

        Impl::ZMQFrameHandler frameHandler;
        int ret = 0;

#ifndef DIRECT_TO_PUB
        notifyBroker.Start();
#endif
        notifySubscriber.Start();

        // sub/pub bind/connect is synchronous, slight delay to ensure connectivity
        this_thread::sleep_for( chrono::milliseconds( 300 ) );

        // ret = notifySubscriber.Subscribe(
        //     vector< NotificationType >{ "" },
        //     chrono::milliseconds( 1 ) );

        for ( auto it = notificationCounts.cbegin(); it != notificationCounts.cend(); ++it )
        {
            ZMQMessage* countMsg = zmsg_new();

            frameHandler.append_message_value< int >( countMsg, it->second );

            ret = notifySubscriber.Subscribe(
                vector< NotificationType >{ it->first },
                chrono::milliseconds( 10 ) );

            this_thread::sleep_for( chrono::milliseconds( 20 ) );

            EXPECT_EQ( 0, ret ) << "failed to subscribe[" << it->first << "]";

            ret = notifyPublisher.Publish( it->first, &countMsg );
            EXPECT_EQ( 0, ret ) << "failed to publish count[" << it->first << "]";
            if ( ret != 0 )
            {
                zmsg_destroy( &countMsg );
            }

            this_thread::sleep_for( chrono::milliseconds( 80 ) );
        }


        for ( auto it = notificationsToSend.cbegin(); it != notificationsToSend.cend(); ++it )
        {
            ZMQMessage* notificationMsg = zmsg_new();

            frameHandler.append_message_value< RequestId >( notificationMsg, it->second.first );
            frameHandler.append_message_value< string >( notificationMsg, it->second.second );

            ret = notifyPublisher.Publish( it->first, &notificationMsg );
            EXPECT_EQ( 0, ret ) << "failed to publish msg[" << it->first << "]";
            if ( ret != 0 )
            {
                zmsg_destroy( &notificationMsg );
            }
        }

        this_thread::sleep_for( chrono::milliseconds( 80 ) );

        notifySubscriber.Stop();
#ifndef DIRECT_TO_PUB
        notifyBroker.Stop();
#endif

        EXPECT_EQ( 0, testHandler.badMessagesReceived );
        // EXPECT_EQ( 0, testHandler.extraMessagesReceived );
        EXPECT_EQ( notificationCounts.size(), testHandler.expectedNotifications.size() );
        for ( auto it = notificationCounts.cbegin(); it != notificationCounts.cend(); ++it )
        {
            auto itExpected = testHandler.expectedNotifications.find( it->first );
            auto itNotification = testHandler.notifications.find( it->first );

            EXPECT_NE( itExpected, testHandler.expectedNotifications.end() )
                << "notification count not received, "
                "possibly a timing issue - try increasing ms delay[" << it->first << "]";
            EXPECT_NE( itNotification, testHandler.notifications.end() )
                << "notification msgs not received["
                "possibly a timing issue - try increasing ms delay[" << it->first << "]";

            if ( itExpected != testHandler.expectedNotifications.end() )
            {
                EXPECT_EQ( it->second, itExpected->second )
                    << "notification count not matching[" << it->first << "]";
            }

            if ( itNotification != testHandler.notifications.end() )
            {
                EXPECT_TRUE( (size_t)it->second <= itNotification->second.size() )
                    << "notification msg counts not matching[" << it->first << "]";
            }
        }
    }
}
