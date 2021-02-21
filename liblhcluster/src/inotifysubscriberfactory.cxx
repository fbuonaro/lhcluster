#include <lhcluster/inotifysubscriberfactory.h>
#include <lhcluster_impl/notifysubscriber.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class NotifySubscriberFactory : public INotifySubscriberFactory
    {
        public:
            NotifySubscriberFactory()
            :   INotifySubscriberFactory()
            {
            }

            ~NotifySubscriberFactory()
            {
            }

            std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                std::unique_ptr< INotifyHandler > nh )
            {
                return std::unique_ptr< INotifySubscriber >(
                    new NotifySubscriber(
                        p.GetSelfControllerEndpoint(),
                        p.GetPublisherEndpoint(),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ),
                        std::move( nh ) ) );
            }

            std::unique_ptr< INotifySubscriber > Create(
                const NotifySubscriberParameters& p,
                const std::vector< NotificationType >& _subscriptions,
                std::unique_ptr< INotifyHandler > nh )
            {
                return std::unique_ptr< INotifySubscriber >(
                    new NotifySubscriber(
                        p.GetSelfControllerEndpoint(),
                        p.GetPublisherEndpoint(),
                        _subscriptions,
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ),
                        std::move( nh ) ) );
            }
    };
}

    INotifySubscriberFactory::INotifySubscriberFactory()
    {
    }

    INotifySubscriberFactory::~INotifySubscriberFactory()
    {
    }

    std::unique_ptr< INotifySubscriberFactory > INotifySubscriberFactory::GetDefaultFactory()
    {
        return std::unique_ptr< INotifySubscriberFactory >(
            new Impl::NotifySubscriberFactory() );
    }
}
