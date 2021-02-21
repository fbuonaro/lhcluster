#include <lhcluster/inotifybrokerfactory.h>
#include <lhcluster_impl/notifybroker.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class NotifyBrokerFactory : public INotifyBrokerFactory
    {
        public:
            NotifyBrokerFactory()
            : INotifyBrokerFactory()
            {
            }

            ~NotifyBrokerFactory()
            {
            }

            std::unique_ptr< INotifyBroker > Create( const NotifyBrokerParameters& p )
            {
                return std::unique_ptr< INotifyBroker >(
                    new NotifyBroker(
                        p.GetFrontendMessagingEndpoint(),
                        p.GetBackendMessagingEndpoint() ) );
            }
    };
}

    INotifyBrokerFactory::INotifyBrokerFactory()
    {
    }

    INotifyBrokerFactory::~INotifyBrokerFactory()
    {
    }

    std::unique_ptr< INotifyBrokerFactory > INotifyBrokerFactory::GetDefaultFactory()
    {
        return std::unique_ptr< INotifyBrokerFactory >( new Impl::NotifyBrokerFactory() );
    }
}
