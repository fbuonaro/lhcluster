#include <lhcluster/iclientfactory.h>
#include <lhcluster_impl/client.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class ClientFactory : public IClientFactory
    {
        public:
            ClientFactory()
            : IClientFactory()
            {
            }

            ~ClientFactory()
            {
            }

            std::unique_ptr< IClient > Create( const ClientParameters& p )
            {
                return std::unique_ptr< IClient >(
                    new Client(
                        p.GetClientMessagingEndpoint(),
                        p.GetClientProxyMessagingEndpoint(),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ) ) );
            }
    };
}

    IClientFactory::IClientFactory()
    {
    }

    IClientFactory::~IClientFactory()
    {
    }

    std::unique_ptr< IClientFactory > IClientFactory::GetDefaultFactory()
    {
        return std::unique_ptr< IClientFactory >( new Impl::ClientFactory() );
    }

}
