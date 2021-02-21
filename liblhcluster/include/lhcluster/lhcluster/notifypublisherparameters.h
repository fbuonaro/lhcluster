#ifndef __LHCLUSTER_NOTIFYPUBLISHERPARAMETERS_H__
#define __LHCLUSTER_NOTIFYPUBLISHERPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class NotifyPublisherParameters
    {
        public:
            NotifyPublisherParameters();
            NotifyPublisherParameters(
                const Endpoint& proxyEndpoint,
                const Endpoint& selfEndpoint );

            bool operator==( const NotifyPublisherParameters& rhs ) const
            {
                return ( ( proxyEndpoint == rhs.proxyEndpoint ) &&
                         ( selfEndpoint == rhs.selfEndpoint ) );
            }

            NotifyPublisherParameters& SetProxyEndpoint(
                const Endpoint& _proxyEndpoint )
            {
                proxyEndpoint = _proxyEndpoint;
                return *this;
            }

            const Endpoint& GetProxyEndpoint() const
            {
                return proxyEndpoint;
            }

            NotifyPublisherParameters& SetSelfEndpoint(
                const Endpoint& _selfEndpoint )
            {
                selfEndpoint = _selfEndpoint;
                return *this;
            }

            const Endpoint& GetSelfEndpoint() const
            {
                return selfEndpoint;
            }

        private:
            Endpoint proxyEndpoint;
            Endpoint selfEndpoint;
    };
}

#endif
