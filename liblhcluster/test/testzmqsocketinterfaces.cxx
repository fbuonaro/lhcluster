#include <lhcluster/endpoint.h>

#include "testzmqsocketutils.h"

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;

    class TestLHCluster : public ::testing::Test
    {
        protected:

            void SetUp()
            {

            }

            void Tear()
            {
            }
    };

    TEST_F( TestLHCluster, TestTestZMQSocketFactory )
    {
        TestZMQSocketFactory socketFactory;
        Endpoint endpoint1( EndpointType::InterProcess, "endpoint1" );
        Endpoint endpoint2( EndpointType::InterProcess, "endpoint1" );
        ZMQSocket* socket1 = socketFactory.Create( SocketType::Pair );
        ZMQSocket* socket1b = socket1;
        ZMQSocket* socket2 = socketFactory.Create( SocketType::Pair );
        bool exceptionThrown = false;

        // make sure that test sockets were created
        ASSERT_EQ( 2, socketFactory.sockets.size() ) 
        << "no sockets created";
        ASSERT_EQ( (TestZMQSocket*) socket1, &socketFactory.sockets[ 0 ] )
        << "first socket is not as expected";
        ASSERT_EQ( (TestZMQSocket*) socket2, &socketFactory.sockets[ 1 ] )
        << "second socket is not as expected";

        // Test destroying destroyed throws exception
        ( ( TestZMQSocket* ) socket1 )->destroyed = true;

        try
        {
            socketFactory.Destroy( &socket1 );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_TRUE( exceptionThrown )
        << "no exception thrown after setting destroyed";

        // Test destroying undestroyed destroys
        ( ( TestZMQSocket* ) socket1 )->destroyed = false;

        exceptionThrown = false;
        try
        {
            socketFactory.Destroy( &socket1 );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_FALSE( exceptionThrown )
        << "exception thrown after destroying undestroyed";
        socket1 = socket1b;
        ASSERT_TRUE( ( ( TestZMQSocket* ) socket1 )->destroyed )
        << "socket not destroyed";

        // Test cannot bind/unbind/connect/disconnect destroyed item
        exceptionThrown = false;
        try
        {
            socketFactory.Bind( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_TRUE( exceptionThrown )
        << "no exception thrown after binding destroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.UnBind( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_TRUE( exceptionThrown )
        << "no exception thrown after unbinding destroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.Connect( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_TRUE( exceptionThrown )
        << "no exception thrown after connecting destroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.DisConnect( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_TRUE( exceptionThrown )
        << "no exception thrown after disconnecting destroyed";

        // Test can bind/unbind/connect/disconnect undestroyed item
        ( ( TestZMQSocket* ) socket1 )->destroyed = false;

        exceptionThrown = false;
        try
        {
            socketFactory.Bind( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_FALSE( exceptionThrown )
        << "exception thrown after binding undestroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.UnBind( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_FALSE( exceptionThrown )
        << "exception thrown after unbinding undestroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.Connect( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_FALSE( exceptionThrown )
        << "exception thrown after connecting undestroyed";

        exceptionThrown = false;
        try
        {
            socketFactory.DisConnect( socket1, endpoint1.path() );
        }
        catch( const std::runtime_error& e )
        {
            exceptionThrown = true;
        }
        catch( ... )
        {
            ASSERT_TRUE( false ) << "unexpected exception";
        }
        ASSERT_FALSE( exceptionThrown )
        << "exception thrown after disconnecting undestroyed";

        ASSERT_TRUE(
            ((TestZMQSocket*) socket1)->binded.end() !=
            ((TestZMQSocket*) socket1)->binded.find(
                endpoint1.path() ) ) << "bind did not work";
        ASSERT_TRUE(
            ((TestZMQSocket*) socket1)->unBinded.end() !=
            ((TestZMQSocket*) socket1)->unBinded.find(
                endpoint1.path() ) ) << "unBind did not work";
        ASSERT_TRUE(
            ((TestZMQSocket*) socket1)->connected.end() !=
            ((TestZMQSocket*) socket1)->connected.find(
                endpoint1.path() ) ) << "connect did not work";
        ASSERT_TRUE(
            ((TestZMQSocket*) socket1)->connected.end() !=
            ((TestZMQSocket*) socket1)->disConnected.find(
                endpoint1.path() ) ) << "disConnect did not work";
    }

    TEST_F( TestLHCluster, TestTestZMQSocketSenderReceiver )
    {
        int aSocketInt = 1;
        ZMQSocket* nullSocket = nullptr;
        ZMQSocket* intSocket = ( ZMQSocket* ) &aSocketInt;
        ZMQMessage* nullMessage = nullptr;
        std::vector< ZMQMessage* > receiveReturnValues{ nullptr };
        std::vector< int > sendReturnValues{ 3 };
        std::vector< int > signalReturnValues{ 10, 20 };
        TestZMQSocketSenderReceiver senderReceiver;
        senderReceiver.receiveReturnValues = receiveReturnValues;
        senderReceiver.sendReturnValues = sendReturnValues;
        senderReceiver.signalReturnValues = signalReturnValues;
        int signal1 = 3;
        int signal2 = 5;
        int signal3 = 6;
        int signal1Ret = senderReceiver.Signal( nullSocket, signal1 );
        int signal2Ret = senderReceiver.Signal( nullSocket, signal2 );
        int signal3Ret = senderReceiver.Signal( intSocket, signal3 );
        int send1Ret = senderReceiver.Send( &nullMessage, nullSocket );
        int send2Ret = senderReceiver.Send( &nullMessage, intSocket );
        ZMQMessage* receive1Ret = senderReceiver.Receive( intSocket );
        ZMQMessage* receive2Ret = senderReceiver.Receive( nullSocket );

        ASSERT_EQ( signal1Ret, 10 ) << "singal 1 ret not as expected";
        ASSERT_EQ( signal2Ret, 20 ) << "singal 2 ret not as expected";
        ASSERT_EQ( signal3Ret, 20 ) << "singal 3 ret not same as signal2 ret";

        ASSERT_EQ( 3, senderReceiver.signals.size() ) << "signals not stored";;
        ASSERT_EQ( nullptr, senderReceiver.signals[0].first ) << "wrong socket 1";
        ASSERT_EQ( signal1, senderReceiver.signals[0].second ) << "wrong signal 1";

        ASSERT_EQ( nullptr, senderReceiver.signals[1].first ) << "wrong socket 2";
        ASSERT_EQ( signal2, senderReceiver.signals[1].second ) << "wrong signal 2";

        ASSERT_EQ( (TestZMQSocket*) intSocket,
                   senderReceiver.signals[2].first ) << "wrong socket 3";
        ASSERT_EQ( signal3, senderReceiver.signals[2].second ) << "wrong signal 3";

        ASSERT_EQ( 2, senderReceiver.sentMessages.size() )
        << "sent msgs not stored";
        ASSERT_EQ( send1Ret, 3 ) << "wrong send ret 1";
        ASSERT_EQ( send2Ret, 3 ) << "wrong send ret 2";
        ASSERT_EQ( (TestZMQSocket*) nullSocket,
                   senderReceiver.sentMessages[0].first );
        ASSERT_EQ( nullptr, senderReceiver.sentMessages[0].second );
        ASSERT_EQ( (TestZMQSocket*) intSocket,
                   senderReceiver.sentMessages[1].first );
        ASSERT_EQ( nullptr, senderReceiver.sentMessages[1].second );

        ASSERT_EQ( 2, senderReceiver.receivedFromSockets.size() )
        << "received from sockets not stored";
        ASSERT_EQ( nullptr, receive1Ret ) << "wrong received msg 1";
        ASSERT_EQ( nullptr, receive2Ret ) << "wrong received msg 2";
        ASSERT_EQ( (TestZMQSocket*) intSocket,
                   senderReceiver.receivedFromSockets[ 0 ] )
        << "received from wrong socket 1";
        ASSERT_EQ( (TestZMQSocket*) nullSocket,
                   senderReceiver.receivedFromSockets[ 1 ] )
        << "received from wrong socket 2";
    }

}
