#ifndef __LHCLUSTER_CLIENTPARAMETERS_H__
#define __LHCLUSTER_CLIENTPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class ClientParameters
    {
        public:
            ClientParameters();
            ClientParameters(
                const Endpoint& clientMessagingEndpoint,
                const Endpoint& clientProxyMessagingEndpoint );

            bool operator==( const ClientParameters& rhs ) const
            {
                return ( ( clientMessagingEndpoint == rhs.clientMessagingEndpoint ) &&
                         ( clientProxyMessagingEndpoint == rhs.clientProxyMessagingEndpoint ) );
            }

            ClientParameters& SetClientMessagingEndpoint(
                const Endpoint& _clientMessagingEndpoint )
            {
                clientMessagingEndpoint = _clientMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetClientMessagingEndpoint() const
            {
                return clientMessagingEndpoint;
            }

            ClientParameters& SetClientProxyMessagingEndpoint(
                const Endpoint& _clientProxyMessagingEndpoint )
            {
                clientProxyMessagingEndpoint = _clientProxyMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetClientProxyMessagingEndpoint() const
            {
                return clientProxyMessagingEndpoint;
            }

        private:
            Endpoint clientMessagingEndpoint;
            Endpoint clientProxyMessagingEndpoint;
    };
}

#endif
