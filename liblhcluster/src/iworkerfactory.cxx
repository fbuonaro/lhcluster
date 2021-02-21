#include <lhcluster/iworkerfactory.h>
#include <lhcluster_impl/worker.h>
#include <lhcluster_impl/zmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
namespace Impl
{
    class WorkerFactory : public IWorkerFactory
    {
        public:
            WorkerFactory()
            :   IWorkerFactory()
            {
            }

            ~WorkerFactory()
            {
            }

            std::unique_ptr< IWorker > Create(
                const WorkerParameters& p,
                std::unique_ptr< IRequestHandler > rh )
            {
                return std::unique_ptr< IWorker >(
                    new Worker(
                        p.GetSelfMessagingEndpoint(),
                        p.GetBrokerMessagingEndpoint(),
                        p.GetSelfControllerEndpoint(),
                        p.GetSelfHeartbeatDelay(),
                        p.GetInitialBrokerHeartbeatDelay(),
                        p.GetBrokerPulseDelay(),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ),
                        p.GetRequestHandlerInternalEndpoint(),
                        p.GetRequestHandlerControllerEndpoint(),
                        std::move( rh ),
                        std::unique_ptr< IZMQSocketFactory >( new ZMQSocketFactory() ),
                        std::unique_ptr< IZMQSocketSenderReceiver >(
                            new ZMQSocketSenderReceiver() ) ) );
            }
    };
}

    IWorkerFactory::IWorkerFactory()
    {
    }

    IWorkerFactory::~IWorkerFactory()
    {
    }

    std::unique_ptr< IWorkerFactory > IWorkerFactory::GetDefaultFactory()
    {
        return std::unique_ptr< IWorkerFactory >( new Impl::WorkerFactory() );
    }
}
