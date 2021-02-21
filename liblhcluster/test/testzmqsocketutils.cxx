#include "testzmqsocketutils.h"

#include <exception>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;

    TestZMQSocketFactory::TestZMQSocketFactory()
    : socketsCreated( 0 )
    , socketsDestroyed( 0 )
    , numBinds( 0 )
    , numUnBinds( 0 )
    , numConnects( 0 )
    , numDisConnects( 0 )
    , numSubscribes( 0 )
    , numUnSubscribes( 0 )
    , sockets()
    , bindReturnValues()
    , unBindReturnValues()
    , connectReturnValues()
    , disConnectReturnValues()
    {
        // Create will return pointers to sockets.data()
        // we cannot have reallocations
        sockets.reserve( 1024 ); 
        bindReturnValues.push_back( 0 );
        unBindReturnValues.push_back( 0 );
        connectReturnValues.push_back( 0 );
        disConnectReturnValues.push_back( 0 );
    }

    ZMQSocket* TestZMQSocketFactory::Create( SocketType socketType )
    {
        TestZMQSocket* data = sockets.data();
        sockets.emplace_back( socketType );
        if( data != sockets.data() )
            throw std::runtime_error(
                "reallocation in sockets data, tests will "
                "break because all created 'sockets' are now invalid" );
        return
            ( ZMQSocket* ) ( sockets.data() +
                             socketsCreated++);
    }

    void TestZMQSocketFactory::Destroy( ZMQSocket** zmqSocket )
    {
        TestZMQSocket* ds = ( TestZMQSocket* ) *zmqSocket;

        if( ds->destroyed )
            throw std::runtime_error( "same socket destroyed twice" );

        ds->destroyed = true;
        ++socketsDestroyed;
        *zmqSocket = nullptr;
        return;
    }

    int TestZMQSocketFactory::Bind( ZMQSocket* zmqSocket,
                                    const char* endpoint )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        int ret = 0;

        if( testZMQSocket->destroyed )
            throw std::runtime_error( "binding destroyed socket" );

        if( numBinds >= bindReturnValues.size() )
            ret = bindReturnValues.back();
        else
            ret = bindReturnValues[ numBinds++ ];

        if( ret == 0 )
            testZMQSocket->binded.insert( endpoint );

        return ret;
    }

    int TestZMQSocketFactory::UnBind( ZMQSocket* zmqSocket,
                                      const char* endpoint )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        int ret = 0;

        if( testZMQSocket->destroyed )
            throw std::runtime_error( "unbinding destroyed socket" );

        if( numUnBinds >= unBindReturnValues.size() )
            ret = unBindReturnValues.back();
        else
            ret = unBindReturnValues[ numUnBinds++ ];

        if( ret == 0 )
            testZMQSocket->unBinded.insert( endpoint );

        return ret;
    }

    int TestZMQSocketFactory::Connect( ZMQSocket* zmqSocket,
                                       const char* endpoint )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        int ret = 0;

        if( testZMQSocket->destroyed )
            throw std::runtime_error( "connecting destroyed socket" );

        if( numConnects >= connectReturnValues.size() )
            ret = connectReturnValues.back();
        else
            ret = connectReturnValues[ numConnects++ ];

        if( ret == 0 )
            testZMQSocket->connected.insert( endpoint );

        return ret;
    }

    int TestZMQSocketFactory::DisConnect( ZMQSocket* zmqSocket,
                                          const char* endpoint )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        int ret = 0;

        if( testZMQSocket->destroyed )
            throw std::runtime_error( "disconnecting destroyed socket" );

        if( numDisConnects >= disConnectReturnValues.size() )
            ret = disConnectReturnValues.back();
        else
            ret = disConnectReturnValues[ numDisConnects++ ];

        if( ret == 0 )
            testZMQSocket->disConnected.insert( endpoint );

        return ret;
    }

    void TestZMQSocketFactory::Subscribe( ZMQSocket* zmqSocket,
                                          const char* stringNType )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        testZMQSocket->subscribedTo.insert( stringNType );
        ++numSubscribes;
    }

    void TestZMQSocketFactory::UnSubscribe( ZMQSocket* zmqSocket,
                                            const char* stringNType )
    {
        TestZMQSocket* testZMQSocket = ( TestZMQSocket* ) zmqSocket;
        testZMQSocket->unsubscribedFrom.insert( stringNType );
        ++numUnSubscribes;
    }


    TestZMQSocketSenderReceiver::TestZMQSocketSenderReceiver()
    : receiveCount( 0 )
    , sendCount( 0 )
    , signalCount( 0 )
    , sentMessages()
    , signals()
    , receivedFromSockets()
    , receiveReturnValues()
    , sendReturnValues()
    , signalReturnValues()
    {
        receiveReturnValues.push_back( nullptr );
        sendReturnValues.push_back( 0 );
        signalReturnValues.push_back( 0 );
    }

    TestZMQSocketSenderReceiver::TestZMQSocketSenderReceiver(
        const TestZMQSocketSenderReceiver& other )
    : receiveCount( other.receiveCount )
    , sendCount( other.sendCount )
    , signalCount( other.signalCount )
    , sentMessages()
    , signals( other.signals )
    , receivedFromSockets( other.receivedFromSockets )
    , receiveReturnValues()
    , sendReturnValues( other.sendReturnValues )
    , signalReturnValues( other.signalReturnValues )
    {
        for( auto it = other.receiveReturnValues.begin();
             it != other.receiveReturnValues.end();
             ++it )
        {
            ZMQMessage* zmqMessage = *it;
            if( zmqMessage )
                receiveReturnValues.push_back( zmsg_dup( zmqMessage ) );
        }

        for( auto it = other.sentMessages.begin();
             it != other.sentMessages.end();
             ++it )
        {
            ZMQMessage* zmqMessage = it->second;
            if( zmqMessage )
                sentMessages.emplace_back(
                    it->first, zmsg_dup( zmqMessage ) );
        }
    }

    TestZMQSocketSenderReceiver::TestZMQSocketSenderReceiver(
        TestZMQSocketSenderReceiver&& other )
    :   TestZMQSocketSenderReceiver()
    {
        swap( *this, other );
    }

    TestZMQSocketSenderReceiver&
    TestZMQSocketSenderReceiver::operator=( TestZMQSocketSenderReceiver other )
    {
        swap( *this, other );

        return *this;
    }

    TestZMQSocketSenderReceiver::~TestZMQSocketSenderReceiver()
    {
        for( auto it = receiveReturnValues.begin();
             it != receiveReturnValues.end();
             ++it )
        {
            ZMQMessage* zmqMessage = *it;
            *it = nullptr;
            if( zmqMessage )
                zmsg_destroy( &zmqMessage );
        }

        for( auto it = sentMessages.begin();
             it != sentMessages.end();
             ++it )
        {
            ZMQMessage* zmqMessage = it->second;
            it->second = nullptr;
            if( zmqMessage )
                zmsg_destroy( &zmqMessage );
        }
    }

    ZMQMessage* TestZMQSocketSenderReceiver::Receive( ZMQSocket* zmqSocket )
    {
        ZMQMessage* ret = nullptr;

        if( receiveCount >= receiveReturnValues.size() )
            ret = receiveReturnValues.back();
        else
            ret = receiveReturnValues[ receiveCount ];

        if( ret )
            ret = zmsg_dup( ret );
        receivedFromSockets.push_back( ( TestZMQSocket*) zmqSocket );

        ++receiveCount;

        return ret;
    }

    int TestZMQSocketSenderReceiver::Send( ZMQMessage** zmqMessage, ZMQSocket* zmqSocket )
    {
        int ret = 0;

        if( sendCount >= sendReturnValues.size() )
        {
            ret = sendReturnValues.back();
        }
        else
            ret = sendReturnValues[ sendCount ];

        if( !ret )
        {
            ZMQMessage* dup = zmsg_dup( *zmqMessage );
            sentMessages.emplace_back( ( TestZMQSocket*) zmqSocket, dup );
            zmsg_destroy( zmqMessage );
            *zmqMessage = nullptr;
        }
        else
        {
            // failure, do not touch message
            sentMessages.emplace_back( ( TestZMQSocket*) zmqSocket, nullptr );
        }

        ++sendCount;

        return ret;
    }

    int TestZMQSocketSenderReceiver::Signal( ZMQSocket* zmqSocket, int signal )
    {
        int ret = 0;

        if( signalCount >= signalReturnValues.size() )
            ret = signalReturnValues.back();
        else
            ret = signalReturnValues[ signalCount ];

        signals.emplace_back( ( TestZMQSocket*) zmqSocket, signal );

        ++signalCount;

        return ret;
    }
}
