#ifndef __LHCLUSTER_NOTIFYBROKERPARAMETERS_H__
#define __LHCLUSTER_NOTIFYBROKERPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class NotifyBrokerParameters
    {
        public:
            NotifyBrokerParameters();
            NotifyBrokerParameters(
                const Endpoint& frontendMessagingEndpoint,
                const Endpoint& backendMessagingEndpoint );

            bool operator==( const NotifyBrokerParameters& rhs ) const
            {
                return ( ( frontendMessagingEndpoint == rhs.frontendMessagingEndpoint ) &&
                         ( backendMessagingEndpoint == rhs.backendMessagingEndpoint ) );
            }

            NotifyBrokerParameters& SetFrontendMessagingEndpoint(
                const Endpoint& _frontendMessagingEndpoint )
            {
                frontendMessagingEndpoint = _frontendMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetFrontendMessagingEndpoint() const
            {
                return frontendMessagingEndpoint;
            }

            NotifyBrokerParameters& SetBackendMessagingEndpoint(
                const Endpoint& _backendMessagingEndpoint )
            {
                backendMessagingEndpoint = _backendMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetBackendMessagingEndpoint() const
            {
                return backendMessagingEndpoint;
            }

        private:
            Endpoint frontendMessagingEndpoint;
            Endpoint backendMessagingEndpoint;
    };
}

#endif
