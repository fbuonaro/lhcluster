#ifndef __LHCLUSTER_NOTIFYPUBLISHERPARAMETERSBUILDER_H__
#define __LHCLUSTER_NOTIFYPUBLISHERPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/notifypublisherparameters.h>

namespace LHClusterNS
{
    class NotifyPublisherParametersBuilder
    :   public ClusterParametersBuilder<
            NotifyPublisherParametersBuilder, NotifyPublisherParameters >
    {
        public:
            using ClusterParametersBuilder<
                NotifyPublisherParametersBuilder, NotifyPublisherParameters >::Build;

            NotifyPublisherParametersBuilder();
            ~NotifyPublisherParametersBuilder();

            bool OK() const;
            NotifyPublisherParameters Build( std::ostringstream& oss ) const;

            NotifyPublisherParametersBuilder& SetProxyEndpoint(
                const Endpoint& _proxyEndpoint )
            {
                proxyEndpointBuilder.SetEndpoint( _proxyEndpoint );
                return *this;
            }

            const Endpoint& GetProxyEndpoint() const
            {
                return proxyEndpointBuilder.GetEndpoint();
            }

            NotifyPublisherParametersBuilder& SetSelfEndpointType(
                EndpointType _selfEndpointType )
            {
                selfEndpointBuilder.SetEndpointType( _selfEndpointType );
                return *this;
            }

            EndpointType GetSelfEndpointType() const
            {
                return selfEndpointBuilder.GetEndpointType();
            }

            NotifyPublisherParametersBuilder& SetSelfEndpointPort(
                int port )
            {
                selfEndpointBuilder.SetPort( port );
                return *this;
            }

            int GetSelfEndpointPort() const
            {
                return selfEndpointBuilder.GetPort();
            }

            NotifyPublisherParametersBuilder& SetSelfEndpointBaseAddress(
                const std::string& _baseAddress )
            {
                selfEndpointBuilder.SetBaseAddress( _baseAddress );
                return *this;
            }

            const std::string& GetSelfEndpointBaseAddress() const
            {
                return selfEndpointBuilder.GetBaseAddress();
            }

        private:
            EndpointBuilder proxyEndpointBuilder;
            EndpointBuilder selfEndpointBuilder;
    };
}

#endif
