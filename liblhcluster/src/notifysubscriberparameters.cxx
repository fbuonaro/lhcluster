#include <lhcluster/notifysubscriberparameters.h>

namespace LHClusterNS
{
    NotifySubscriberParameters::NotifySubscriberParameters()
    :   publisherEndpoint()
    {
    }

    NotifySubscriberParameters::NotifySubscriberParameters(
        const Endpoint& _selfControllerEndpoint,
        const Endpoint& _publisherEndpoint )
    :   selfControllerEndpoint( _selfControllerEndpoint )
    ,   publisherEndpoint( _publisherEndpoint )
    {
    }
}
