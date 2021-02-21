#include <lhcluster/ibrokerfactory.h>
#include <lhcluster_impl/broker.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class BrokerFactory : public IBrokerFactory
    {
        public:
            BrokerFactory()
            : IBrokerFactory()
            {
            }

            ~BrokerFactory()
            {
            }

            std::unique_ptr< IBroker > Create( const BrokerParameters& p )
            {
                return std::unique_ptr< IBroker >(
                    new Broker(
                        p.GetFrontendMessagingEndpoint(),
                        p.GetBackendMessagingEndpoint(),
                        p.GetControlEndpoint(),
                        p.GetSelfHeartbeatDelay(),
                        p.GetMinimumWorkerHeartbeatDelay(),
                        p.GetWorkerPulseDelay(),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ) ) );
            }
    };
}

    IBrokerFactory::IBrokerFactory()
    {
    }

    IBrokerFactory::~IBrokerFactory()
    {
    }

    std::unique_ptr< IBrokerFactory > IBrokerFactory::GetDefaultFactory()
    {
        return std::unique_ptr< IBrokerFactory >( new Impl::BrokerFactory() );
    }

}
