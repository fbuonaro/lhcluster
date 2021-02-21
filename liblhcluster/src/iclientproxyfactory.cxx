#include <lhcluster/iclientproxyfactory.h>
#include <lhcluster_impl/clientproxy.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class ClientProxyFactory : public IClientProxyFactory
    {
        public:
            ClientProxyFactory()
            : IClientProxyFactory()
            {
            }

            ~ClientProxyFactory()
            {
            }

            std::unique_ptr< IClientProxy > Create( const ClientProxyParameters& p )
            {
                return std::unique_ptr< IClientProxy >(
                    new ClientProxy(
                        p.GetSelfBrokerMessagingEndpoint(),
                        p.GetBrokerMessagingEndpoint(),
                        p.GetSelfControllerEndpoint(),
                        p.GetSelfClientMessagingEndpoint(),
                        p.GetSelfHeartbeatDelay(),
                        p.GetInitialBrokerHeartbeatDelay(),
                        p.GetBrokerPulseDelay(),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ) ) );
            }
    };
}

    IClientProxyFactory::IClientProxyFactory()
    {
    }

    IClientProxyFactory::~IClientProxyFactory()
    {
    }

    std::unique_ptr< IClientProxyFactory > IClientProxyFactory::GetDefaultFactory()
    {
        return std::unique_ptr< IClientProxyFactory >( new Impl::ClientProxyFactory() );
    }

}
