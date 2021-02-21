#ifndef __LHCLUSTER_CLIENTPROXYPARAMETERS_H__
#define __LHCLUSTER_CLIENTPROXYPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class ClientProxyParameters
    {
        public:
            ClientProxyParameters();
            ClientProxyParameters(
                const Endpoint& selfBrokerMessagingEndpoint,
                const Endpoint& brokerMessagingEndpoint,
                const Endpoint& selfControllerEndpoint,
                const Endpoint& selfClientMessagingEndpoint,
                Delay selfHeartbeatDelay,
                Delay initialBrokerHeartbeatDelay,
                Delay brokerPulseDelay );

            bool operator==( const ClientProxyParameters& rhs ) const
            {
                return ( ( selfBrokerMessagingEndpoint == rhs.selfBrokerMessagingEndpoint ) &&
                         ( brokerMessagingEndpoint == rhs.brokerMessagingEndpoint ) &&
                         ( selfControllerEndpoint == rhs.selfControllerEndpoint ) &&
                         ( selfClientMessagingEndpoint == rhs.selfClientMessagingEndpoint ) &&
                         ( selfHeartbeatDelay == rhs.selfHeartbeatDelay ) &&
                         ( initialBrokerHeartbeatDelay == rhs.initialBrokerHeartbeatDelay ) &&
                         ( brokerPulseDelay == rhs.brokerPulseDelay ) );
            }

            ClientProxyParameters& SetSelfBrokerMessagingEndpoint(
                const Endpoint& _selfBrokerMessagingEndpoint )
            {
                selfBrokerMessagingEndpoint = _selfBrokerMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetSelfBrokerMessagingEndpoint() const
            {
                return selfBrokerMessagingEndpoint;
            }

            ClientProxyParameters& SetBrokerMessagingEndpoint(
                const Endpoint& _brokerMessagingEndpoint )
            {
                brokerMessagingEndpoint = _brokerMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetBrokerMessagingEndpoint() const
            {
                return brokerMessagingEndpoint;
            }

            ClientProxyParameters& SetSelfControllerEndpoint(
                const Endpoint& _selfControllerEndpoint )
            {
                selfControllerEndpoint = _selfControllerEndpoint;
                return *this;
            }

            const Endpoint& GetSelfControllerEndpoint() const
            {
                return selfControllerEndpoint;
            }

            ClientProxyParameters& SetSelfClientMessagingEndpoint(
                const Endpoint& _selfClientMessagingEndpoint )
            {
                selfClientMessagingEndpoint = _selfClientMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetSelfClientMessagingEndpoint() const
            {
                return selfClientMessagingEndpoint;
            }

            ClientProxyParameters& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }

            ClientProxyParameters& SetInitialBrokerHeartbeatDelay(
                Delay _initialBrokerHeartbeatDelay )
            {
                initialBrokerHeartbeatDelay = _initialBrokerHeartbeatDelay;
                return *this;
            }

            Delay GetInitialBrokerHeartbeatDelay() const
            {
                return initialBrokerHeartbeatDelay;
            }

            ClientProxyParameters& SetBrokerPulseDelay( Delay _brokerPulseDelay )
            {
                brokerPulseDelay = _brokerPulseDelay;
                return *this;
            }

            Delay GetBrokerPulseDelay() const
            {
                return brokerPulseDelay;
            }


        private:
            Endpoint selfBrokerMessagingEndpoint;
            Endpoint brokerMessagingEndpoint;
            Endpoint selfControllerEndpoint;
            Endpoint selfClientMessagingEndpoint;
            Delay selfHeartbeatDelay;
            Delay initialBrokerHeartbeatDelay;
            Delay brokerPulseDelay;
    };
}

#endif
