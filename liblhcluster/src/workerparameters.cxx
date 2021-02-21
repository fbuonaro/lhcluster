#include <lhcluster/workerparameters.h>

namespace LHClusterNS
{
    WorkerParameters::WorkerParameters()
    :   selfMessagingEndpoint()
    ,   brokerMessagingEndpoint()
    ,   selfControllerEndpoint()
    ,   selfInternalEndpoint()
    ,   requestHandlerInternalEndpoint()
    ,   requestHandlerControllerEndpoint()
    ,   selfHeartbeatDelay( 0 )
    ,   initialBrokerHeartbeatDelay( 0 )
    ,   brokerPulseDelay( 0 )
    {
    }

    WorkerParameters::WorkerParameters(
        const Endpoint& _selfMessagingEndpoint,
        const Endpoint& _brokerMessagingEndpoint,
        const Endpoint& _selfControllerEndpoint,
        const Endpoint& _selfInternalEndpoint,
        const Endpoint& _requestHandlerInternalEndpoint,
        const Endpoint& _requestHandlerControllerEndpoint,
        Delay _selfHeartbeatDelay,
        Delay _initialBrokerHeartbeatDelay,
        Delay _brokerPulseDelay )
    :   selfMessagingEndpoint( _selfMessagingEndpoint )
    ,   brokerMessagingEndpoint( _brokerMessagingEndpoint )
    ,   selfControllerEndpoint( _selfControllerEndpoint )
    ,   selfInternalEndpoint( _selfInternalEndpoint )
    ,   requestHandlerInternalEndpoint( _requestHandlerInternalEndpoint )
    ,   requestHandlerControllerEndpoint( _requestHandlerControllerEndpoint )
    ,   selfHeartbeatDelay( _selfHeartbeatDelay )
    ,   initialBrokerHeartbeatDelay( _initialBrokerHeartbeatDelay )
    ,   brokerPulseDelay( _brokerPulseDelay )
    {
    }
}
