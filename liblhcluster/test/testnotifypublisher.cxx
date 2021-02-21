#include <lhcluster_impl/notifypublisher.h>
#include <lhcluster_impl/zmqframehandler.h>

#include "testzmqsocketutils.h"

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;

    class TestNotifyPublisher : public ::testing::Test
    {
        protected:

            void SetUp()
            {

            }

            void Tear()
            {
            }
    };

    TEST_F( TestNotifyPublisher, TestProxyConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointProxy( EndpointType::InterProcess, "proxy" );
        TestZMQSocket* publisherSocket = nullptr;
        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        NotifyPublisher notifyPublisher( endpointProxy,
                                         std::move( lpSocketFactory ),
                                         std::move( lpSenderReceiver ) );

        publisherSocket =
            ( TestZMQSocket* ) notifyPublisher.selfPublisherSocket;

        notifyPublisher.activatePublisherSocket();

        ASSERT_FALSE(  publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 0, publisherSocket->binded.size() );
        ASSERT_EQ( 0, publisherSocket->unBinded.size() );
        ASSERT_EQ( 1, publisherSocket->connected.size() );
        ASSERT_EQ( 0, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->connected.find( endpointProxy.path() ) !=
                     publisherSocket->connected.end() );

        }

        ASSERT_TRUE( publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 0, publisherSocket->binded.size() );
        ASSERT_EQ( 0, publisherSocket->unBinded.size() );
        ASSERT_EQ( 1, publisherSocket->connected.size() );
        ASSERT_EQ( 1, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->connected.find(
                        endpointProxy.path() ) !=
                     publisherSocket->connected.end() );
        ASSERT_TRUE( publisherSocket->disConnected.find(
                        endpointProxy.path() ) !=
                     publisherSocket->disConnected.end() );
    }

    TEST_F( TestNotifyPublisher, TestSelfConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointSelf( EndpointType::InterProcess, "self" );
        TestZMQSocket* publisherSocket = nullptr;
        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        NotifyPublisher notifyPublisher( endpointSelf, true,
                                         std::move( lpSocketFactory ),
                                         std::move( lpSenderReceiver ) );

        publisherSocket =
            ( TestZMQSocket* ) notifyPublisher.selfPublisherSocket;

        notifyPublisher.activatePublisherSocket();

        ASSERT_FALSE(  publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 1, publisherSocket->binded.size() );
        ASSERT_EQ( 0, publisherSocket->unBinded.size() );
        ASSERT_EQ( 0, publisherSocket->connected.size() );
        ASSERT_EQ( 0, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->binded.find( endpointSelf.path() ) !=
                     publisherSocket->binded.end() );
        }

        ASSERT_TRUE( publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 1, publisherSocket->binded.size() );
        ASSERT_EQ( 1, publisherSocket->unBinded.size() );
        ASSERT_EQ( 0, publisherSocket->connected.size() );
        ASSERT_EQ( 0, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->binded.find(
                        endpointSelf.path() ) !=
                     publisherSocket->binded.end() );
        ASSERT_TRUE( publisherSocket->unBinded.find(
                        endpointSelf.path() ) !=
                     publisherSocket->unBinded.end() );
    }

    TEST_F( TestNotifyPublisher, TestBothSelfProxyConstructorActivate )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointSelf( EndpointType::InterProcess, "self" );
        Endpoint endpointProxy( EndpointType::InterProcess, "proxy" );
        TestZMQSocket* publisherSocket = nullptr;
        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        NotifyPublisher notifyPublisher( endpointSelf, endpointProxy,
                                         std::move( lpSocketFactory ),
                                         std::move( lpSenderReceiver ) );

        publisherSocket =
            ( TestZMQSocket* ) notifyPublisher.selfPublisherSocket;

        notifyPublisher.activatePublisherSocket();

        ASSERT_FALSE(  publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 1, publisherSocket->binded.size() );
        ASSERT_EQ( 0, publisherSocket->unBinded.size() );
        ASSERT_EQ( 1, publisherSocket->connected.size() );
        ASSERT_EQ( 0, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->binded.find( endpointSelf.path() ) !=
                     publisherSocket->binded.end() );
        ASSERT_TRUE( publisherSocket->connected.find( endpointProxy.path() ) !=
                     publisherSocket->connected.end() );

        }

        ASSERT_TRUE( publisherSocket->destroyed );
        ASSERT_EQ( SocketType::Publisher, publisherSocket->socketType );
        ASSERT_EQ( 1, publisherSocket->binded.size() );
        ASSERT_EQ( 1, publisherSocket->unBinded.size() );
        ASSERT_EQ( 1, publisherSocket->connected.size() );
        ASSERT_EQ( 1, publisherSocket->disConnected.size() );
        ASSERT_TRUE( publisherSocket->binded.find(
                        endpointSelf.path() ) !=
                     publisherSocket->binded.end() );
        ASSERT_TRUE( publisherSocket->connected.find(
                        endpointProxy.path() ) !=
                     publisherSocket->connected.end() );
        ASSERT_TRUE( publisherSocket->unBinded.find(
                        endpointSelf.path() ) !=
                     publisherSocket->unBinded.end() );
        ASSERT_TRUE( publisherSocket->disConnected.find(
                        endpointProxy.path() ) !=
                     publisherSocket->disConnected.end() );
    }

    TEST_F( TestNotifyPublisher, TestPublishBasic )
    {
        TestZMQSocketFactory socketFactory;
        TestZMQSocketSenderReceiver senderReceiver;
        Endpoint endpointSelf( EndpointType::InterProcess, "self" );
        Endpoint endpointProxy( EndpointType::InterProcess, "proxy" );
        TestZMQSocket* publisherSocket = nullptr;
        ZMQFrameHandler frameHandler;
        ZMQFrame* frame = nullptr;
        ZMQMessage* pubMsg = zmsg_new();
        ZMQMessage* pubMsg2 = nullptr;
        int ret = 0;

        frameHandler.append_message_value( pubMsg, std::string( "hello world" ) );
        
        {
        std::unique_ptr< IZMQSocketFactory > lpSocketFactory(
            new TestZMQSocketFactoryProxy( socketFactory ) );
        std::unique_ptr< IZMQSocketSenderReceiver > lpSenderReceiver(
            new TestZMQSocketSenderReceiverProxy( senderReceiver ) );
        NotifyPublisher notifyPublisher( endpointSelf, endpointProxy,
                                         std::move( lpSocketFactory ),
                                         std::move( lpSenderReceiver ) );

        publisherSocket =
            ( TestZMQSocket* ) notifyPublisher.selfPublisherSocket;

        notifyPublisher.activatePublisherSocket();
        ret= notifyPublisher.Publish( "50", static_cast< RequestId >( 300 ) );
        ASSERT_EQ( 0, ret );
        ret = notifyPublisher.Publish( "80", static_cast< RequestId >( 0 ) ); //rtype == 80, rid == 0
        ASSERT_EQ( 0, ret );
        ret = notifyPublisher.Publish( "0", static_cast< RequestId >( 3 ) ); //rtype == 0, rid == 3
        ASSERT_EQ( 0, ret );
        pubMsg2 = zmsg_dup( pubMsg );
        ret = notifyPublisher.Publish( "30", &pubMsg2 );
        ASSERT_EQ( 0, ret );
        pubMsg2 = zmsg_dup( pubMsg );
        ret = notifyPublisher.Publish( "30", 2, &pubMsg2 );
        ASSERT_EQ( 0, ret );
        }

        ASSERT_EQ( 5, senderReceiver.sendCount );
        ASSERT_EQ( 0, senderReceiver.receiveCount );
        ASSERT_EQ( 0, senderReceiver.signalCount );


#define checkBasicPublisherMessage( frame, frameHandler, msg, rType, rId ) \
        ASSERT_EQ( 2, zmsg_size( msg ) ); \
        frame = zmsg_first( msg ); \
        ASSERT_TRUE( zframe_size( frame ) > 0 ); \
        ASSERT_EQ( rType, \
                   frameHandler.get_frame_value_as_str< NotificationType >( frame ) ); \
        frame = zmsg_next( msg ); \
        ASSERT_TRUE( zframe_size( frame ) > 0 ); \
        ASSERT_EQ( rId, \
                   frameHandler.get_frame_value< RequestId >( frame ) )

        ASSERT_EQ( publisherSocket, senderReceiver.sentMessages[0].first );
        checkBasicPublisherMessage(
            frame,
            frameHandler,
            senderReceiver.sentMessages[0].second,
            "50",
            300 );

        ASSERT_EQ( publisherSocket, senderReceiver.sentMessages[1].first );
        checkBasicPublisherMessage(
            frame,
            frameHandler,
            senderReceiver.sentMessages[1].second,
            "80",
            0 );

        ASSERT_EQ( publisherSocket, senderReceiver.sentMessages[2].first );
        checkBasicPublisherMessage(
            frame,
            frameHandler,
            senderReceiver.sentMessages[2].second,
            "0",
            3 );

        ASSERT_EQ( publisherSocket, senderReceiver.sentMessages[3].first );
        ASSERT_EQ( 2, zmsg_size(senderReceiver.sentMessages[3].second) );
        frame = zmsg_first(senderReceiver.sentMessages[3].second);
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( "30",
                   frameHandler.get_frame_value_as_str< NotificationType >( frame ) );
        frame = zmsg_next(senderReceiver.sentMessages[3].second);
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( std::string( "hello world" ),
                   frameHandler.get_frame_value< std::string >( frame ) );

        ASSERT_EQ( publisherSocket, senderReceiver.sentMessages[4].first );
        ASSERT_EQ( 3, zmsg_size(senderReceiver.sentMessages[4].second) );
        frame = zmsg_first(senderReceiver.sentMessages[4].second);
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( "30",
                   frameHandler.get_frame_value_as_str< NotificationType >( frame ) );
        frame = zmsg_next(senderReceiver.sentMessages[4].second);
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( 2,
                   frameHandler.get_frame_value< RequestId >( frame ) );
        frame = zmsg_next(senderReceiver.sentMessages[4].second);
        ASSERT_TRUE( zframe_size( frame ) > 0 );
        ASSERT_EQ( std::string( "hello world" ),
                   frameHandler.get_frame_value< std::string >( frame ) );
    }

}
