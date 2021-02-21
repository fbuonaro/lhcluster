#include <lhcluster/inotifyhandler.h>
#include <lhcluster_impl/notifysubscriber.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "testzmqsocketutils.h"
#include "test.h"

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;
    using namespace std;

    class TestNotifySubscriber : public ::testing::Test
    {
        protected:

            void SetUp()
            {

            }

            void Tear()
            {
            }
    };

    class TestNotifyHandler : public INotifyHandler
    {
        public:
            // DATA
            vector< pair< NotificationType, ZMQMessage* > > processedMessages;
            vector< RequestStatus > processRetValues;

            // CTORS, DTORS
            TestNotifyHandler()
            : INotifyHandler()
            , processedMessages()
            , processRetValues()
            {
                processRetValues.push_back( RequestStatus::InProgress );
            }

            ~TestNotifyHandler()
            {
                for( auto it = processedMessages.begin();
                     it != processedMessages.end();
                     ++it )
                {
                    ZMQMessage* zmqMessage = it->second;
                    it->second = nullptr;
                    zmsg_destroy( &zmqMessage );
                }
            }

            RequestStatus Process( const NotificationType& notificationType ,
                                   ZMQMessage* notificationMsg )
            {
                RequestStatus ret = RequestStatus::InProgress;

                if( processedMessages.size() < processRetValues.size() )
                    ret = processRetValues[ processedMessages.size() ];
                else
                    ret = processRetValues.back();

                processedMessages.emplace_back( notificationType,
                                                zmsg_dup( notificationMsg ) );

                return ret;
            }
    };

    class TestNotifyHandlerProxy : public INotifyHandler
    {
        public:
            TestNotifyHandlerProxy( TestNotifyHandler& _notifyHandler )
            : INotifyHandler()
            , notifyHandler( _notifyHandler )
            {}

            RequestStatus Process( const NotificationType& notificationType ,
                                   ZMQMessage* notificationMsg )
            {
                return notifyHandler.Process( notificationType, notificationMsg );
            }

         private:
            TestNotifyHandler& notifyHandler;
    };

    TEST_F( TestNotifySubscriber, TestConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint publisherEndpoint( EndpointType::TCP, "publisher" );
        vector< NotificationType > subs{ "1", "2", "3", "300" };
        TestNotifyHandler notifyHandler;
        TestZMQSocket* subSocket = nullptr;
        TestZMQSocket* listenerSocket = nullptr;
        TestZMQSocket* signalerSocket = nullptr;

        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        std::unique_ptr< TestNotifyHandlerProxy > lpNotifyHandler(
            new TestNotifyHandlerProxy( notifyHandler ) );

        NotifySubscriber notifySubscriber( selfControllerEndpoint,
                                           publisherEndpoint,
                                           subs,
                                           move( lpSocketFactory ),
                                           move( lpSenderReceiver ),
                                           move( lpNotifyHandler ) );

        // should connect to publisherEndpoint
        subSocket = ( TestZMQSocket* ) notifySubscriber.selfSubscriberSocket;
        // should bind to controller endpoint
        listenerSocket = ( TestZMQSocket* ) notifySubscriber.listenerSocket;
        // should connect to controller endpoint
        signalerSocket = ( TestZMQSocket* ) notifySubscriber.signalerSocket;

        notifySubscriber.activateControllerSockets();
        notifySubscriber.activateSubscriberSockets();

        ASSERT_EQ( SocketType::Subscriber, subSocket->socketType );
        ASSERT_EQ( 4, subSocket->subscribedTo.size() );
        ASSERT_EQ( 1, subSocket->connected.size() );
        ASSERT_EQ( 0, subSocket->disConnected.size() );
        ASSERT_TRUE( subSocket->connected.find(
                        publisherEndpoint.path() ) !=
                     subSocket->connected.end() );

        ASSERT_EQ( SocketType::Pair, listenerSocket->socketType );
        ASSERT_EQ( 1, listenerSocket->binded.size() );
        ASSERT_EQ( 0, listenerSocket->unBinded.size() );
        ASSERT_TRUE( listenerSocket->binded.find(
                        selfControllerEndpoint.path() ) !=
                     listenerSocket->binded.end() );

        ASSERT_EQ( SocketType::Pair, signalerSocket->socketType );
        ASSERT_EQ( 1, signalerSocket->connected.size() );
        ASSERT_EQ( 0, signalerSocket->disConnected.size() );
        ASSERT_TRUE( signalerSocket->connected.find(
                        selfControllerEndpoint.path() ) !=
                     signalerSocket->connected.end() );

        // only called when exiting StartAsPrimary, must call manually
        notifySubscriber.deactivateSubscriberSockets();
        }

        ASSERT_TRUE( subSocket->destroyed );
        ASSERT_EQ( SocketType::Subscriber, subSocket->socketType );
        ASSERT_EQ( 4, subSocket->subscribedTo.size() );
        ASSERT_EQ( 1, subSocket->connected.size() );
        ASSERT_EQ( 1, subSocket->disConnected.size() );
        ASSERT_TRUE( subSocket->disConnected.find(
                        publisherEndpoint.path() ) !=
                     subSocket->disConnected.end() );

        ASSERT_TRUE( listenerSocket->destroyed );
        ASSERT_EQ( 1, listenerSocket->binded.size() );
        ASSERT_EQ( 1, listenerSocket->unBinded.size() );
        ASSERT_TRUE( listenerSocket->unBinded.find(
                        selfControllerEndpoint.path() ) !=
                     listenerSocket->unBinded.end() );

        ASSERT_TRUE( signalerSocket->destroyed );
        ASSERT_EQ( 1, signalerSocket->connected.size() );
        ASSERT_EQ( 1, signalerSocket->disConnected.size() );
        ASSERT_TRUE( signalerSocket->disConnected.find(
                        selfControllerEndpoint.path() ) !=
                     signalerSocket->disConnected.end() );
    }

    TEST_F( TestNotifySubscriber, TestHandlePublisherNotification )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint selfControllerEndpoint( EndpointType::TCP, "controller" );
        Endpoint publisherEndpoint( EndpointType::TCP, "publisher" );
        vector< NotificationType > subs{ "1", "2", "3", "300", "3000" };
        TestNotifyHandler notifyHandler;
        TestZMQSocket* subSocket = nullptr;
        TestZMQSocket* listenerSocket = nullptr;
        TestZMQSocket* signalerSocket = nullptr;
        ZMQMessage* nullMessage = nullptr;
        ZMQMessage* nullFrameMessage = nullptr;
        ZMQMessage* goodMessage = nullptr;
        ZMQMessage* goodMessageFatal = nullptr;
        ZMQMessage* goodMessage2 = nullptr;
        ZMQMessage* goodMessage3 = nullptr;
        vector< RequestStatus > processRetValues {
            RequestStatus::Failed,
            RequestStatus::FatalError,
            RequestStatus::Succeeded,
            RequestStatus::InProgress };
        vector< ZMQMessage* > receiveReturnValues;
        ZMQFrameHandler frameHandler;

        // add messages to receive
        // good message must contain at least a single 
        // frame with a NotificationType and may contain additional frames afterward
        // Success stats: NotificationsReceived, NotificationsProcessed
        // Three exceptional cases:
        //  1) Receive returns nullptr -> FailedPublisherReceives++
        //  2) No NotificationType (null frame)
        //  3) process returns non-zero -> NotificationsFailed++
        
        // The cases will be 
        // 1) bad receive 2) bad message (no notify type) 3) bad process
        // 4) process good message with just not type 5) msg with more data

        nullFrameMessage = zmsg_new();
        zmsg_pushmem( nullFrameMessage, NULL, 0 );

        goodMessage = zmsg_new();
        frameHandler.append_message_value_as_str< NotificationType >(
            goodMessage,
            "300" );

        goodMessageFatal = zmsg_new();
        frameHandler.append_message_value_as_str< NotificationType >(
            goodMessageFatal,
            "3000" );

        goodMessage2 = zmsg_new();
        frameHandler.append_message_value_as_str< NotificationType >(
            goodMessage2,
            "2" );

        goodMessage3 = zmsg_new();
        frameHandler.append_message_value_as_str< NotificationType >(
            goodMessage3,
            "1" );
        frameHandler.append_message_value< RequestId >(
            goodMessage3,
            4000000 );

        receiveReturnValues = {
            nullMessage,
            nullFrameMessage,
            goodMessage,
            goodMessageFatal,
            goodMessage2,
            goodMessage3
        };

        notifyHandler.processRetValues = processRetValues;
        senderReceiver.receiveReturnValues = receiveReturnValues;

        {
        int ret = 0;
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        std::unique_ptr< TestNotifyHandlerProxy > lpNotifyHandler(
            new TestNotifyHandlerProxy( notifyHandler ) );

        NotifySubscriber notifySubscriber( selfControllerEndpoint,
                                           publisherEndpoint,
                                           subs,
                                           move( lpSocketFactory ),
                                           move( lpSenderReceiver ),
                                           move( lpNotifyHandler ) );

        // should connect to publisherEndpoint
        subSocket = ( TestZMQSocket* ) notifySubscriber.selfSubscriberSocket;
        // should bind to controller endpoint
        listenerSocket = ( TestZMQSocket* ) notifySubscriber.listenerSocket;
        // should connect to controller endpoint
        signalerSocket = ( TestZMQSocket* ) notifySubscriber.signalerSocket;

        notifySubscriber.activateControllerSockets();
        notifySubscriber.activateSubscriberSockets();

        // 1) null msg returned
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( 0, ret ) << "null msg returned non-zero ret";
        STATS_ASSERT_EQ( 1,
                         notifySubscriber.stats[
                         NotifySubscriberStat::FailedPublisherReceives]);

        // 2) null notify type frame
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( 1, ret ) << "null notify frame returned zero ret";
        STATS_ASSERT_EQ( 1,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsReceived]);

        // 3) process failed but continues, good message
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( 0, ret ) << "process failed returned non-zero ret";
        STATS_ASSERT_EQ( 1,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsFailed]);
        STATS_ASSERT_EQ( 2,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsReceived]);
        ASSERT_EQ( 1, notifyHandler.processedMessages.size() );
        ASSERT_EQ( "300", notifyHandler.processedMessages.back().first );
        ASSERT_EQ( 0,
                   zmsg_size( notifyHandler.processedMessages.back().second ) );

        // 4) process failed and halts, good message
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( -1, ret ) << "process fatal failed returned zero ret";
        STATS_ASSERT_EQ( 2,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsFailed]);
        STATS_ASSERT_EQ( 3,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsReceived]);
        ASSERT_EQ( 2, notifyHandler.processedMessages.size() );
        ASSERT_EQ( "3000", notifyHandler.processedMessages.back().first );
        ASSERT_EQ( 0,
                   zmsg_size( notifyHandler.processedMessages.back().second ) );

        // 5) process succeeded, good message trigger unsub
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( 0, ret ) << "process good msg returned non-zero ret";
        STATS_ASSERT_EQ( 1,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsProcessed]);
        STATS_ASSERT_EQ( 4,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsReceived]);
        ASSERT_EQ( 3, notifyHandler.processedMessages.size() );
        ASSERT_EQ( "2", notifyHandler.processedMessages.back().first );
        ASSERT_EQ( 0,
                   zmsg_size( notifyHandler.processedMessages.back().second ) );
        ASSERT_EQ( 1, socketFactory.numUnSubscribes );
        ASSERT_TRUE(
            subSocket->unsubscribedFrom.find( "2" ) != subSocket->unsubscribedFrom.cend() );

        // 6) process succeeded, good message with extra data, no unsub
        ret = notifySubscriber.handlePublisherNotification();
        ASSERT_EQ( 0, ret ) << "process good msg+ returned non-zero ret";
        STATS_ASSERT_EQ( 2,
                   notifySubscriber.stats[
                   NotifySubscriberStat::NotificationsProcessed]);
        STATS_ASSERT_EQ( 5,
                         notifySubscriber.stats[
                         NotifySubscriberStat::NotificationsReceived]);
        ASSERT_EQ( 4, notifyHandler.processedMessages.size() );
        ASSERT_EQ( "1", notifyHandler.processedMessages.back().first );
        ASSERT_EQ( 1,
                   zmsg_size( notifyHandler.processedMessages.back().second ) );
        ASSERT_EQ( 4000000,
                   frameHandler.get_frame_value< RequestId >(
                    zmsg_first(
                        notifyHandler.processedMessages.back().second ) ) );


        // only called when exiting StartAsPrimary, must call manually
        notifySubscriber.deactivateSubscriberSockets();

        STATS_ASSERT_EQ( 1,
                   notifySubscriber.stats[
                   NotifySubscriberStat::FailedPublisherReceives]);
        STATS_ASSERT_EQ( 2,
                   notifySubscriber.stats[
                   NotifySubscriberStat::NotificationsFailed]);
        ASSERT_EQ( 6, senderReceiver.receiveCount );
        }

        ASSERT_EQ( 1, socketFactory.numUnSubscribes ); // TODO - do not unsub on deactivate
                                                       //        not sure if matters
    }

    // TODO - test commands for sub/unsub
}
