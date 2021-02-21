#include <lhcluster/clientproxyparameters.h>

namespace LHClusterNS
{
    ClientProxyParameters::ClientProxyParameters()
    :   selfBrokerMessagingEndpoint()
    ,   brokerMessagingEndpoint()
    ,   selfControllerEndpoint()
    ,   selfClientMessagingEndpoint()
    ,   selfHeartbeatDelay( 0 )
    ,   initialBrokerHeartbeatDelay( 0 )
    ,   brokerPulseDelay( 0 )
    {
    }

    ClientProxyParameters::ClientProxyParameters(
        const Endpoint& _selfBrokerMessagingEndpoint,
        const Endpoint& _brokerMessagingEndpoint,
        const Endpoint& _selfControllerEndpoint,
        const Endpoint& _selfClientMessagingEndpoint,
        Delay _selfHeartbeatDelay,
        Delay _initialBrokerHeartbeatDelay,
        Delay _brokerPulseDelay )
    :   selfBrokerMessagingEndpoint( _selfBrokerMessagingEndpoint )
    ,   brokerMessagingEndpoint( _brokerMessagingEndpoint )
    ,   selfControllerEndpoint( _selfControllerEndpoint )
    ,   selfClientMessagingEndpoint( _selfClientMessagingEndpoint )
    ,   selfHeartbeatDelay( _selfHeartbeatDelay )
    ,   initialBrokerHeartbeatDelay( _initialBrokerHeartbeatDelay )
    ,   brokerPulseDelay( _brokerPulseDelay )
    {
    }
}
