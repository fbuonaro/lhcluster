#ifndef __LHCLUSTER_WORKERPARAMETERS_H__
#define __LHCLUSTER_WORKERPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class WorkerParameters
    {
        public:
            WorkerParameters();
            WorkerParameters(
                const Endpoint& selfMessagingEndpoint,
                const Endpoint& brokerMessagingEndpoint,
                const Endpoint& selfControllerEndpoint,
                const Endpoint& selfInternalEndpoint,
                const Endpoint& requestHandlerInternalEndpoint,
                const Endpoint& requestHandlerControllerEndpoint,
                Delay selfHeartbeatDelay,
                Delay initialBrokerHeartbeatDelay,
                Delay brokerPulseDelay );

            bool operator==( const WorkerParameters& rhs ) const
            {
                return ( ( selfMessagingEndpoint == rhs.selfMessagingEndpoint ) &&
                         ( brokerMessagingEndpoint == rhs.brokerMessagingEndpoint ) &&
                         ( selfControllerEndpoint == rhs.selfControllerEndpoint ) &&
                         ( selfInternalEndpoint == rhs.selfInternalEndpoint ) &&
                         ( requestHandlerInternalEndpoint == rhs.requestHandlerInternalEndpoint ) &&
                         ( requestHandlerControllerEndpoint == rhs.requestHandlerControllerEndpoint ) &&
                         ( selfHeartbeatDelay == rhs.selfHeartbeatDelay ) &&
                         ( initialBrokerHeartbeatDelay == rhs.initialBrokerHeartbeatDelay ) &&
                         ( brokerPulseDelay == rhs.brokerPulseDelay ) );
            }

            WorkerParameters& SetSelfMessagingEndpoint( const Endpoint& _selfMessagingEndpoint )
            {
                selfMessagingEndpoint = _selfMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetSelfMessagingEndpoint() const
            {
                return selfMessagingEndpoint;
            }

            WorkerParameters& SetBrokerMessagingEndpoint( const Endpoint& _brokerMessagingEndpoint )
            {
                brokerMessagingEndpoint = _brokerMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetBrokerMessagingEndpoint() const
            {
                return brokerMessagingEndpoint;
            }

            WorkerParameters& SetSelfControllerEndpoint( const Endpoint& _selfControllerEndpoint )
            {
                selfControllerEndpoint = _selfControllerEndpoint;
                return *this;
            }

            const Endpoint& GetSelfControllerEndpoint() const
            {
                return selfControllerEndpoint;
            }

            WorkerParameters& SetSelfInternalEndpoint( const Endpoint& _selfInternalEndpoint )
            {
                selfInternalEndpoint = _selfInternalEndpoint;
                return *this;
            }

            const Endpoint& GetSelfInternalEndpoint() const
            {
                return selfInternalEndpoint;
            }

            WorkerParameters& SetRequestHandlerInternalEndpoint(
                const Endpoint& _requestHandlerInternalEndpoint )
            {
                requestHandlerInternalEndpoint = _requestHandlerInternalEndpoint;
                return *this;
            }

            const Endpoint& GetRequestHandlerInternalEndpoint() const
            {
                return requestHandlerInternalEndpoint;
            }

            WorkerParameters& SetRequestHandlerControllerEndpoint(
                const Endpoint& _requestHandlerControllerEndpoint )
            {
                requestHandlerControllerEndpoint = _requestHandlerControllerEndpoint;
                return *this;
            }

            const Endpoint& GetRequestHandlerControllerEndpoint() const
            {
                return requestHandlerControllerEndpoint;
            }

            WorkerParameters& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }

            WorkerParameters& SetInitialBrokerHeartbeatDelay( Delay _initialBrokerHeartbeatDelay )
            {
                initialBrokerHeartbeatDelay = _initialBrokerHeartbeatDelay;
                return *this;
            }

            Delay GetInitialBrokerHeartbeatDelay() const
            {
                return initialBrokerHeartbeatDelay;
            }

            WorkerParameters& SetBrokerPulseDelay( Delay _brokerPulseDelay )
            {
                brokerPulseDelay = _brokerPulseDelay;
                return *this;
            }

            Delay GetBrokerPulseDelay() const
            {
                return brokerPulseDelay;
            }

        private:
            Endpoint selfMessagingEndpoint;
            Endpoint brokerMessagingEndpoint;
            Endpoint selfControllerEndpoint;
            Endpoint selfInternalEndpoint;
            Endpoint requestHandlerInternalEndpoint;
            Endpoint requestHandlerControllerEndpoint;
            Delay selfHeartbeatDelay;
            Delay initialBrokerHeartbeatDelay;
            Delay brokerPulseDelay;

    };
}

#endif
