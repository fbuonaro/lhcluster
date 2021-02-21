#ifndef __LHCLUSTER_CLIENTPARAMETERSBUILDER_H__
#define __LHCLUSTER_CLIENTPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/clientparameters.h>

namespace LHClusterNS
{
    class ClientParametersBuilder
    :   public ClusterParametersBuilder< ClientParametersBuilder, ClientParameters >
    {
        public:
            using ClusterParametersBuilder<
                ClientParametersBuilder, ClientParameters >::Build;

            ClientParametersBuilder();
            ~ClientParametersBuilder();

            ClientParameters Build( std::ostringstream& oss ) const;
            bool OK() const;

            ClientParametersBuilder& SetClientEndpointType( EndpointType _clientEndpointType )
            {
                clientEndpointBuilder.SetEndpointType( _clientEndpointType );
                return *this;
            }

            EndpointType GetClientEndpointType() const
            {
                return clientEndpointBuilder.GetEndpointType();
            }

            ClientParametersBuilder& SetClientEndpointBaseAddress(
                const std::string& _clientEndpointBaseAddress )
            {
                clientEndpointBuilder.SetBaseAddress( _clientEndpointBaseAddress );
                return *this;
            }

            const std::string& GetClientEndpointBaseAddress() const
            {
                return clientEndpointBuilder.GetBaseAddress();
            }

            ClientParametersBuilder& SetClientEndpointPort( int _clientEndpointPort )
            {
                clientEndpointBuilder.SetPort( _clientEndpointPort );
                return *this;
            }

            int GetClientEndpointPort() const
            {
                return clientEndpointBuilder.GetPort();
            }

            ClientParametersBuilder& SetClientProxyEndpoint(
                const Endpoint& _clientProxyEndpoint )
            {
                clientProxyEndpointBuilder.SetEndpoint( _clientProxyEndpoint );
                return *this;
            }

            const Endpoint& GetClientProxyEndpoint() const
            {
                return clientProxyEndpointBuilder.GetEndpoint();
            }

        private:
            EndpointBuilder clientEndpointBuilder;
            EndpointBuilder clientProxyEndpointBuilder;
    };
}

#endif
