#ifndef __LHCLUSTER_BROKERPARAMETERS_H__
#define __LHCLUSTER_BROKERPARAMETERS_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>

namespace LHClusterNS
{
    class BrokerParameters
    {
        public:
            BrokerParameters();
            BrokerParameters(
                const Endpoint& frontendMessagingEndpoint,
                const Endpoint& backendMessagingEndpoint,
                const Endpoint& controlEndpoint,
                Delay selfHeartbeatDelay,
                Delay minimumWorkerHeartbeatDelay,
                Delay workerPulseDelay );

            bool operator==( const BrokerParameters& rhs ) const
            {
                return ( ( frontendMessagingEndpoint == rhs.frontendMessagingEndpoint ) &&
                         ( backendMessagingEndpoint == rhs.backendMessagingEndpoint ) &&
                         ( controlEndpoint == rhs.controlEndpoint ) &&
                         ( selfHeartbeatDelay == rhs.selfHeartbeatDelay ) &&
                         ( minimumWorkerHeartbeatDelay == rhs.minimumWorkerHeartbeatDelay ) &&
                         ( workerPulseDelay == rhs.workerPulseDelay ) );
            }

            BrokerParameters& SetFrontendMessagingEndpoint(
                const Endpoint& _frontendMessagingEndpoint )
            {
                frontendMessagingEndpoint = _frontendMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetFrontendMessagingEndpoint() const
            {
                return frontendMessagingEndpoint;
            }
 
            BrokerParameters& SetBackendMessagingEndpoint(
                const Endpoint& _backendMessagingEndpoint )
            {
                backendMessagingEndpoint = _backendMessagingEndpoint;
                return *this;
            }

            const Endpoint& GetBackendMessagingEndpoint() const
            {
                return backendMessagingEndpoint;
            }
  
            BrokerParameters& SetControlEndpoint( const Endpoint& _controlEndpoint )
            {
                controlEndpoint = _controlEndpoint;
                return *this;
            }

            const Endpoint& GetControlEndpoint() const
            {
                return controlEndpoint;
            }
           
            BrokerParameters& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }
           
            BrokerParameters& SetMinimumWorkerHeartbeatDelay(
                Delay _minimumWorkerHeartbeatDelay )
            {
                minimumWorkerHeartbeatDelay = _minimumWorkerHeartbeatDelay;
                return *this;
            }

            Delay GetMinimumWorkerHeartbeatDelay() const
            {
                return minimumWorkerHeartbeatDelay;
            }
  
            BrokerParameters& SetWorkerPulseDelay( Delay _workerPulseDelay )
            {
                workerPulseDelay = _workerPulseDelay;
                return *this;
            }

            Delay GetWorkerPulseDelay() const
            {
                return workerPulseDelay;
            }


        private:
            Endpoint frontendMessagingEndpoint; //where client proxy connects to
            Endpoint backendMessagingEndpoint;  //where workers connect to
            Endpoint controlEndpoint;           //where the controller thread connects to
            Delay selfHeartbeatDelay;           //how often the broker sends a heartbeat
            Delay minimumWorkerHeartbeatDelay;  //how often the worker should send a heartbeat
            Delay workerPulseDelay;             //how often the worker is checked for a pulse
    };
}

#endif
