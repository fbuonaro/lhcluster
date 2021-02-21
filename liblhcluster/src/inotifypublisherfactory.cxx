#include <lhcluster/inotifypublisherfactory.h>
#include <lhcluster_impl/notifypublisher.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class NotifyPublisherFactory : public INotifyPublisherFactory
    {
        public:
            NotifyPublisherFactory()
            : INotifyPublisherFactory()
            {
            }

            ~NotifyPublisherFactory()
            {
            }

            std::unique_ptr< INotifyPublisher > Create( const NotifyPublisherParameters& p )
            {
                if( p.GetProxyEndpoint().type() != EndpointType::None )
                {
                    if( p.GetSelfEndpoint().type() != EndpointType::None )
                        return std::unique_ptr< INotifyPublisher >(
                            new NotifyPublisher(
                                p.GetSelfEndpoint(),
                                p.GetProxyEndpoint(),
                                std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                                std::unique_ptr< IZMQSocketSenderReceiver >(
                                    new ZMQSocketSenderReceiver() ) ) );
                    else
                        return std::unique_ptr< INotifyPublisher >(
                            new NotifyPublisher(
                                p.GetProxyEndpoint(),
                                std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                                std::unique_ptr< IZMQSocketSenderReceiver >(
                                    new ZMQSocketSenderReceiver() ) ) );
                }
                else
                    return std::unique_ptr< INotifyPublisher >(
                        new NotifyPublisher(
                            p.GetSelfEndpoint(),
                            true,
                            std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                            std::unique_ptr< IZMQSocketSenderReceiver >(
                                new ZMQSocketSenderReceiver() ) ) );

            }
    };
}

    INotifyPublisherFactory::INotifyPublisherFactory()
    {
    }

    INotifyPublisherFactory::~INotifyPublisherFactory()
    {
    }

    std::unique_ptr< INotifyPublisherFactory > INotifyPublisherFactory::GetDefaultFactory()
    {
        return std::unique_ptr< INotifyPublisherFactory >( new Impl::NotifyPublisherFactory() );
    }
}
