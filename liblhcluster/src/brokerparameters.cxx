#include <lhcluster/brokerparameters.h>

namespace LHClusterNS
{
    BrokerParameters::BrokerParameters()
    :   frontendMessagingEndpoint()
    ,   backendMessagingEndpoint()
    ,   controlEndpoint()
    ,   selfHeartbeatDelay( 0 )
    ,   minimumWorkerHeartbeatDelay( 0 )
    ,   workerPulseDelay( 0 )
    {
    }

    BrokerParameters::BrokerParameters(
        const Endpoint& _frontendMessagingEndpoint,
        const Endpoint& _backendMessagingEndpoint,
        const Endpoint& _controlEndpoint,
        Delay _selfHeartbeatDelay,
        Delay _minimumWorkerHeartbeatDelay,
        Delay _workerPulseDelay )
    :   frontendMessagingEndpoint( _frontendMessagingEndpoint )
    ,   backendMessagingEndpoint( _backendMessagingEndpoint )
    ,   controlEndpoint( _controlEndpoint )
    ,   selfHeartbeatDelay( _selfHeartbeatDelay )
    ,   minimumWorkerHeartbeatDelay( _minimumWorkerHeartbeatDelay )
    ,   workerPulseDelay( _workerPulseDelay )
    {
    }
}
