#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>

#include <set>
#include <string>
#include <utility>
#include <vector>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace LHClusterNS::Impl;

    class TestZMQSocket
    {
        public:
            // DATA
            bool destroyed;
            SocketType socketType;
            std::set< std::string > binded;
            std::set< std::string > unBinded;
            std::set< std::string > connected;
            std::set< std::string > disConnected;
            std::set< std::string > subscribedTo;
            std::set< std::string > unsubscribedFrom;
            
            // CTORS, DTORS
            TestZMQSocket( SocketType _socketType )
            : destroyed( false )
            , socketType( _socketType )
            , binded()
            , unBinded()
            , connected()
            , disConnected()
            , subscribedTo()
            , unsubscribedFrom()
            {
            }
    };

    class TestZMQSocketFactory : public IZMQSocketFactory
    {
        public:
            // DATA
            // sockets created
            unsigned int socketsCreated;
            unsigned int socketsDestroyed;
            unsigned int numBinds;
            unsigned int numUnBinds;
            unsigned int numConnects;
            unsigned int numDisConnects;
            unsigned int numSubscribes;
            unsigned int numUnSubscribes;
            std::vector< TestZMQSocket > sockets;
            std::vector< int > bindReturnValues;
            std::vector< int > unBindReturnValues;
            std::vector< int > connectReturnValues;
            std::vector< int > disConnectReturnValues;

            // CTORS, DTORS
            TestZMQSocketFactory();

            // METHODS
            ZMQSocket* Create( SocketType socketType );
            void Destroy( ZMQSocket** zmqSocket );
            int Bind( ZMQSocket* zmqSocket,
                      const char* endpoint );
            int UnBind( ZMQSocket* zmqSocket,
                        const char* endpoint );
            int Connect( ZMQSocket* zmqSocket,
                         const char* endpoint );
            int DisConnect( ZMQSocket* zmqSocket,
                            const char* endpoint );
            void Subscribe( ZMQSocket* zmqSocket,
                            const char* stringNotifyType );
            void UnSubscribe( ZMQSocket* zmqSocket,
                              const char* stringNotifyType );

    };

    class TestZMQSocketSenderReceiver : public IZMQSocketSenderReceiver
    {
        public:
            // DATA
            unsigned int receiveCount;
            unsigned int sendCount;
            unsigned int signalCount;
            std::vector< std::pair< TestZMQSocket*, ZMQMessage* > > sentMessages;
            std::vector< std::pair< TestZMQSocket*, int > > signals;
            std::vector< TestZMQSocket* > receivedFromSockets;
            // return last if called >size times
            std::vector< ZMQMessage* > receiveReturnValues; 
            std::vector< int > sendReturnValues; 
            std::vector< int > signalReturnValues;

            // CTORS, DTORS
            TestZMQSocketSenderReceiver();
            TestZMQSocketSenderReceiver( const TestZMQSocketSenderReceiver& other );
            TestZMQSocketSenderReceiver( TestZMQSocketSenderReceiver&& other );
            TestZMQSocketSenderReceiver& operator=(
                TestZMQSocketSenderReceiver other );
            ~TestZMQSocketSenderReceiver();

            // METHODS
            friend void swap( TestZMQSocketSenderReceiver& a,
                              TestZMQSocketSenderReceiver& b )
            {

                using std::swap;

                swap( a.receiveCount, b.receiveCount );
                swap( a.sendCount, b.sendCount );
                swap( a.signalCount, b.signalCount );
                swap( a.sentMessages, b.sentMessages );
                swap( a.signals, b.signals );
                swap( a.receivedFromSockets, b.receivedFromSockets );
                swap( a.receiveReturnValues, b.receiveReturnValues );
                swap( a.sendReturnValues, b.sendReturnValues );
                swap( a.signalReturnValues, b.signalReturnValues );
            }

            ZMQMessage* Receive( ZMQSocket* zmqSocket );
            int Send( ZMQMessage** zmqMessage, ZMQSocket* zmqSocket );
            int Signal( ZMQSocket* zmqSocket, int signal );
    };

    class TestZMQSocketSenderReceiverProxy :
        public IZMQSocketSenderReceiver
    {
        public:
            TestZMQSocketSenderReceiverProxy(
                TestZMQSocketSenderReceiver& _senderReceiver )
            : senderReceiver( _senderReceiver )
            {
            }
                
            ZMQMessage* Receive( ZMQSocket* zmqSocket )
            {
                return senderReceiver.Receive( zmqSocket );
            }

            int Send( ZMQMessage** zmqMessage, ZMQSocket* zmqSocket )
            {
                return senderReceiver.Send( zmqMessage, zmqSocket );
            }

            int Signal( ZMQSocket* zmqSocket, int signal )
            {
                return senderReceiver.Signal( zmqSocket, signal );
            }

        private:
            TestZMQSocketSenderReceiver& senderReceiver;
    };

    class TestZMQSocketFactoryProxy :
        public IZMQSocketFactory
    {
        public:
            TestZMQSocketFactoryProxy(
                TestZMQSocketFactory& _socketFactory )
            : socketFactory( _socketFactory )
            {
            }

            ZMQSocket* Create( SocketType socketType )
            {
                return socketFactory.Create( socketType );
            }

            void Destroy( ZMQSocket** zmqSocket )
            {
                socketFactory.Destroy( zmqSocket );
            }

            int Bind( ZMQSocket* zmqSocket,
                      const char* endpoint )
            {
                return socketFactory.Bind( zmqSocket, endpoint );
            }

            int UnBind( ZMQSocket* zmqSocket,
                        const char* endpoint )
            {
                return socketFactory.UnBind( zmqSocket, endpoint );
            }

            int Connect( ZMQSocket* zmqSocket,
                         const char* endpoint )
            {
                return socketFactory.Connect( zmqSocket, endpoint );
            }

            int DisConnect( ZMQSocket* zmqSocket,
                            const char* endpoint )
            {
                return socketFactory.DisConnect( zmqSocket, endpoint );
            }

            void Subscribe( ZMQSocket* zmqSocket,
                            const char* stringNotifyType )
            {
                socketFactory.Subscribe( zmqSocket, stringNotifyType );
            }

            void UnSubscribe( ZMQSocket* zmqSocket,
                              const char* stringNotifyType )
            {
                socketFactory.UnSubscribe( zmqSocket, stringNotifyType );
            }

        private:
            TestZMQSocketFactory& socketFactory;
    };
}
