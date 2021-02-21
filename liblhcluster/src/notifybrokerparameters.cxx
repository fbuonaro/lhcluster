#include <lhcluster/notifybrokerparameters.h>

namespace LHClusterNS
{
    NotifyBrokerParameters::NotifyBrokerParameters()
    :   frontendMessagingEndpoint()
    ,   backendMessagingEndpoint()
    {
    }

    NotifyBrokerParameters::NotifyBrokerParameters(
        const Endpoint& _frontendMessagingEndpoint,
        const Endpoint& _backendMessagingEndpoint )
    :   frontendMessagingEndpoint( _frontendMessagingEndpoint )
    ,   backendMessagingEndpoint( _backendMessagingEndpoint )
    {
    }
}
