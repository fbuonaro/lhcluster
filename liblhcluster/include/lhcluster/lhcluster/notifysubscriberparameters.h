#ifndef __LHCLUSTER_NOTIFYSUBSCRIBERPARAMETERS_H__
#define __LHCLUSTER_NOTIFYSUBSCRIBERPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class NotifySubscriberParameters
    {
        public:
            NotifySubscriberParameters();
            NotifySubscriberParameters(
                const Endpoint& selfControllerEndpoint,
                const Endpoint& publisherEndpoint );

            bool operator==( const NotifySubscriberParameters& rhs ) const
            {
                return ( ( selfControllerEndpoint == rhs.selfControllerEndpoint ) &&
                         ( publisherEndpoint == rhs.publisherEndpoint ) );
            }

            NotifySubscriberParameters& SetSelfControllerEndpoint(
                const Endpoint& _selfControllerEndpoint )
            {
                selfControllerEndpoint = _selfControllerEndpoint;
                return *this;
            }

            const Endpoint& GetSelfControllerEndpoint() const
            {
                return selfControllerEndpoint;
            }

            NotifySubscriberParameters& SetPublisherEndpoint(
                const Endpoint& _publisherEndpoint )
            {
                publisherEndpoint = _publisherEndpoint;
                return *this;
            }

            const Endpoint& GetPublisherEndpoint() const
            {
                return publisherEndpoint;
            }

        private:
            Endpoint selfControllerEndpoint;
            Endpoint publisherEndpoint;
    };
}

#endif
