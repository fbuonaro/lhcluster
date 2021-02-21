#ifndef __LHCLUSTER_CLIENTPROXYPARAMETERSBUILDER_H__
#define __LHCLUSTER_CLIENTPROXYPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/clientproxyparameters.h>

namespace LHClusterNS
{
    class ClientProxyParametersBuilder
    :   public ClusterParametersBuilder< ClientProxyParametersBuilder, ClientProxyParameters >
    {
        public:
            using ClusterParametersBuilder<
                ClientProxyParametersBuilder, ClientProxyParameters >::Build;

            ClientProxyParametersBuilder();
            ~ClientProxyParametersBuilder();

            ClientProxyParameters Build( std::ostringstream& oss ) const;
            bool OK() const;

            ClientProxyParametersBuilder& SetSelfBrokerMessagingEndpointType(
                EndpointType _selfBrokerMessagingEndpointType )
            {
                selfBrokerMessagingEndpointBuilder.SetEndpointType(
                    _selfBrokerMessagingEndpointType );
                return *this;
            }

            EndpointType GetSelfBrokerMessagingEndpointType() const
            {
                return selfBrokerMessagingEndpointBuilder.GetEndpointType();
            }

            ClientProxyParametersBuilder& SetSelfBrokerMessagingEndpointBaseAddress(
                const std::string& _selfBrokerMessagingEndpointBaseAddress )
            {
                selfBrokerMessagingEndpointBuilder.SetBaseAddress(
                    _selfBrokerMessagingEndpointBaseAddress );
                return *this;
            }

            const std::string& GetSelfBrokerMessagingEndpointBaseAddress() const
            {
                return selfBrokerMessagingEndpointBuilder.GetBaseAddress();
            }

            ClientProxyParametersBuilder& SetSelfBrokerMessagingEndpointPort(
                int _selfBrokerMessagingEndpointPort )
            {
                selfBrokerMessagingEndpointBuilder.SetPort(
                    _selfBrokerMessagingEndpointPort );
                return *this;
            }

            int GetSelfBrokerMessagingEndpointPort() const
            {
                return selfBrokerMessagingEndpointBuilder.GetPort();
            }

            ClientProxyParametersBuilder& SetBrokerEndpoint( const Endpoint& _brokerEndpoint )
            {
                brokerEndpointBuilder.SetEndpoint( _brokerEndpoint );
                return *this;
            }

            const Endpoint& GetBrokerEndpoint() const
            {
                return brokerEndpointBuilder.GetEndpoint();
            }

            ClientProxyParametersBuilder& SetSelfClientMessagingEndpointType(
                EndpointType _selfClientMessagingEndpointType )
            {
                selfClientMessagingEndpointBuilder.SetEndpointType(
                    _selfClientMessagingEndpointType );
                return *this;
            }

            EndpointType GetSelfClientMessagingEndpointType() const
            {
                return selfClientMessagingEndpointBuilder.GetEndpointType();
            }

            ClientProxyParametersBuilder& SetSelfClientMessagingEndpointBaseAddress(
                const std::string& _selfClientMessagingEndpointBaseAddress )
            {
                selfClientMessagingEndpointBuilder.SetBaseAddress(
                    _selfClientMessagingEndpointBaseAddress );
                return *this;
            }

            const std::string& GetSelfClientMessagingEndpointBaseAddress() const
            {
                return selfClientMessagingEndpointBuilder.GetBaseAddress();
            }

            ClientProxyParametersBuilder& SetSelfClientMessagingEndpointPort(
                int _selfClientMessagingEndpointPort )
            {
                selfClientMessagingEndpointBuilder.SetPort( _selfClientMessagingEndpointPort );
                return *this;
            }

            int GetSelfClientMessagingEndpointPort() const
            {
                return selfClientMessagingEndpointBuilder.GetPort();
            }

            ClientProxyParametersBuilder& SetControlEndpointBaseAddress(
                const std::string& _controlEndpointBaseAddress )
            {
                controlEndpointBuilder.SetBaseAddress( _controlEndpointBaseAddress );
                return *this;
            }

            const std::string& GetControlEndpointBaseAddress() const
            {
                return controlEndpointBuilder.GetBaseAddress();
            }


            ClientProxyParametersBuilder& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }

            ClientProxyParametersBuilder& SetInitialBrokerHeartbeatDelay(
                Delay _initialBrokerHeartbeatDelay )
            {
                initialBrokerHeartbeatDelay = _initialBrokerHeartbeatDelay;
                return *this;
            }

            Delay GetInitialBrokerHeartbeatDelay() const
            {
                return initialBrokerHeartbeatDelay;
            }

            ClientProxyParametersBuilder& SetBrokerPulseDelay( Delay _brokerPulseDelay )
            {
                brokerPulseDelay = _brokerPulseDelay;
                return *this;
            }

            Delay GetBrokerPulseDelay() const
            {
                return brokerPulseDelay;
            }


        private:
            EndpointBuilder selfBrokerMessagingEndpointBuilder;
            EndpointBuilder brokerEndpointBuilder;
            EndpointBuilder selfClientMessagingEndpointBuilder;
            EndpointBuilder controlEndpointBuilder;

            Delay selfHeartbeatDelay;
            Delay initialBrokerHeartbeatDelay;
            Delay brokerPulseDelay;
    };
}

#endif
