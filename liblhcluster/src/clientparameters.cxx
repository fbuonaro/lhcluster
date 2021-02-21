#include <lhcluster/clientparameters.h>

namespace LHClusterNS
{
    ClientParameters::ClientParameters()
    :   clientMessagingEndpoint()
    ,   clientProxyMessagingEndpoint()
    {
    }

    ClientParameters::ClientParameters(
        const Endpoint& _clientMessagingEndpoint,
        const Endpoint& _clientProxyMessagingEndpoint )
    :   clientMessagingEndpoint( _clientMessagingEndpoint )
    ,   clientProxyMessagingEndpoint( _clientProxyMessagingEndpoint )
    {
    }
}

