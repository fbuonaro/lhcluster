#ifndef __LHCLUSTER_IMPL_NOTIFYPUBLISHER_H__
#define __LHCLUSTER_IMPL_NOTIFYPUBLISHER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/inotifypublisher.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/izmqsocketsenderreceiver.h>
#include <lhcluster_impl/zmqaddin.h>

#include <memory>

namespace LHClusterNS 
{
namespace Impl
{
    class NotifyPublisher : public INotifyPublisher
    {
        public:
            // DATA MEMBERS
            Endpoint selfEndpoint;
            Endpoint proxyEndpoint;
            SocketPublisher* selfPublisherSocket;
            std::unique_ptr< IZMQSocketFactory > socketFactory;
            std::unique_ptr< IZMQSocketSenderReceiver > senderReceiver;

            // CTORS, DTORS
            NotifyPublisher( const NotifyPublisher& other ) = delete;
            NotifyPublisher( NotifyPublisher&& other ) = delete;
            NotifyPublisher& operator=( const NotifyPublisher& other ) = delete;
            NotifyPublisher& operator=( NotifyPublisher&& other ) = delete;
            NotifyPublisher() = delete;

            NotifyPublisher( const Endpoint& _proxyEndpoint,
                             std::unique_ptr< IZMQSocketFactory >
                                _socketFactory,
                             std::unique_ptr< IZMQSocketSenderReceiver > 
                                 _senderReceiver );
            NotifyPublisher( const Endpoint& _selfEndpoint,
                             bool forSelf,
                             std::unique_ptr< IZMQSocketFactory >
                                _socketFactory,
                             std::unique_ptr< IZMQSocketSenderReceiver > 
                                 _senderReceiver );
            NotifyPublisher( const Endpoint& _selfEndpoint,
                             const Endpoint& _proxyEndpoint,
                             std::unique_ptr< IZMQSocketFactory >
                                _socketFactory,
                             std::unique_ptr< IZMQSocketSenderReceiver > 
                                 _senderReceiver );
            ~NotifyPublisher();

            // METHODS
            int Publish( const NotificationType& notifyType );
            int Publish( const NotificationType& notifyType, RequestId requestId );
            int Publish( const NotificationType& notifyType, ZMQMessage** lpZMQMessage );
            int Publish(
                const NotificationType& notifyType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage );
            int activatePublisherSocket();
            int deactivatePublisherSocket();
    };
}
}
#endif
