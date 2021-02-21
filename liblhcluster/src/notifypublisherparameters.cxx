#include <lhcluster/notifypublisherparameters.h>

namespace LHClusterNS
{
    NotifyPublisherParameters::NotifyPublisherParameters()
    :   proxyEndpoint()
    ,   selfEndpoint()
    {
    }

    NotifyPublisherParameters::NotifyPublisherParameters(
        const Endpoint& _proxyEndpoint,
        const Endpoint& _selfEndpoint )
    :   proxyEndpoint( _proxyEndpoint )
    ,   selfEndpoint( _selfEndpoint )
    {
    }
}
