#ifndef __LHCLUSTER_IMPL_NOTIFYSUBSCRIBER_H__
#define __LHCLUSTER_IMPL_NOTIFYSUBSCRIBER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/inotifyhandler.h>
#include <lhcluster/inotifysubscriber.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>
#include <lhcluster_stats/notifysubscriberstats.h>

#include <thread>

namespace LHClusterNS
{
namespace Impl
{
    enum class NotifySubscriberExitState
    {
        None,
        GracefulStop,
        Reconnect,
        StartFailed
    };

    class NotifySubscriber : public INotifySubscriber
    {
        public:
            // DATA
            Endpoint selfControllerEndpoint;
            Endpoint publisherEndpoint;
            std::vector< NotificationType > subscriptions;
            ZMQReactor* reactor;
            SocketPair* listenerSocket;
            SocketPair* signalerSocket;
            SocketSubscriber* selfSubscriberSocket;
            Delay reconnectDelay;
            int publisherReconnectReactorId;
#ifdef STATS
            // id of the timer used to dump stats
            int dumpStatsReactorId;
#endif
            std::unique_ptr< std::thread > reactorThread;
            NotifySubscriberExitState exitState;
#ifdef STATS
            MessageStats< NotifySubscriberStat > stats;
#endif
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;
            std::unique_ptr< INotifyHandler > handler;
            zpoller_t* signalerPoller;

            // CTORS, DTORS
            NotifySubscriber( const NotifySubscriber& other ) = delete;
            NotifySubscriber( NotifySubscriber&& other ) = delete;
            NotifySubscriber& operator=( const NotifySubscriber& other ) = delete;
            NotifySubscriber& operator=( NotifySubscriber&& other ) = delete;
            NotifySubscriber() = delete;

            NotifySubscriber( const Endpoint& _selfControllerEndpoint,
                              const Endpoint& _publisherEndpoint,
                              std::unique_ptr< IZMQSocketFactory >
                                 _socketFactory,
                              std::unique_ptr< IZMQSocketSenderReceiver > 
                                  _senderReceiver,
                              std::unique_ptr< INotifyHandler > _handler );
            NotifySubscriber( const Endpoint& _selfControllerEndpoint,
                              const Endpoint& _publisherEndpoint, 
                              const std::vector< NotificationType >&
                                _subscriptions,
                              std::unique_ptr< IZMQSocketFactory >
                                 _socketFactory,
                              std::unique_ptr< IZMQSocketSenderReceiver > 
                                  _senderReceiver,
                              std::unique_ptr< INotifyHandler > _handler );
            ~NotifySubscriber();

            // METHODS
            void Start();
            void Stop();
            // return 0 on success else non-zero on failure
            int Subscribe( const std::vector< NotificationType >& subscriptions,
                           DelayMS timeout );
            int UnSubscribe( const std::vector< NotificationType >& subscriptions,
                             DelayMS timeout );
            int subOrUnsub( NotifySubscriberCommand subOrUnsub, 
                            const std::vector< NotificationType >& subscriptions,
                            DelayMS timeout );
            int sendCommand( NotifySubscriberCommand command, 
                             DelayMS timeout );
            void StartAsPrimary();
            int handleControllerCommand();
            int handleControllerCommand( ZMQMessage** lpControllerMessage );
            int handlePublisherNotification();
            int setupSubscriberCallbacks();
            int teardownSubscriberCallbacks();
            int activateSubscriberSockets();
            int deactivateSubscriberSockets();
            int activateControllerSockets();
            int deactivateControllerSockets();
            int setupControllerCallbacks();
            int teardownControllerCallbacks();
            void setExitState( NotifySubscriberExitState newExitState );
#ifdef STATS
            int dumpStatsHeaders();
            int dumpStats();
#endif
    };
}
}
    
#endif
