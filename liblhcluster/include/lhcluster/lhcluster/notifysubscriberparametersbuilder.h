#ifndef __LHCLUSTER_NOTIFYSUBSCRIBERPARAMETERSBUILDER_H__
#define __LHCLUSTER_NOTIFYSUBSCRIBERPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/notifysubscriberparameters.h>

namespace LHClusterNS
{
    class NotifySubscriberParametersBuilder
    :   public ClusterParametersBuilder<
            NotifySubscriberParametersBuilder, NotifySubscriberParameters >
    {
        public:
            using ClusterParametersBuilder<
                NotifySubscriberParametersBuilder, NotifySubscriberParameters >::Build;

            NotifySubscriberParametersBuilder();
            ~NotifySubscriberParametersBuilder();

            bool OK() const;
            NotifySubscriberParameters Build( std::ostringstream& oss ) const;

            NotifySubscriberParametersBuilder& SetSelfControllerEndpointBaseAddress(
                const std::string& _frontendEndpointBaseAddress )
            {
                selfControllerEndpointBuilder.SetBaseAddress( _frontendEndpointBaseAddress );
                return *this;
            }

            const std::string& GetSelfControllerEndpointBaseAddress() const
            {
                return selfControllerEndpointBuilder.GetBaseAddress();
            }

            NotifySubscriberParametersBuilder& SetPublisherEndpoint(
                const Endpoint& _backendEndpoint )
            {
                publisherEndpointBuilder.SetEndpoint( _backendEndpoint );
                return *this;
            }

            const Endpoint& GetPublisherEndpoint() const
            {
                return publisherEndpointBuilder.GetEndpoint();
            }

        private:
            EndpointBuilder selfControllerEndpointBuilder;
            EndpointBuilder publisherEndpointBuilder;
    };
}

#endif
