#ifndef __LHCLUSTER_BROKERPARAMETERSBUILDER_H__
#define __LHCLUSTER_BROKERPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/brokerparameters.h>

namespace LHClusterNS
{
    class BrokerParametersBuilder
    :   public ClusterParametersBuilder< BrokerParametersBuilder, BrokerParameters >
    {
        public:
            using ClusterParametersBuilder<
                BrokerParametersBuilder, BrokerParameters >::Build;

            BrokerParametersBuilder();
            ~BrokerParametersBuilder();

            BrokerParameters Build( std::ostringstream& buffer ) const;
            bool OK() const;

            BrokerParametersBuilder& SetFrontendEndpointType( EndpointType _frontendEndpointType )
            {
                frontendEndpointBuilder.SetEndpointType( _frontendEndpointType );
                return *this;
            }

            EndpointType GetFrontendEndpointType() const
            {
                return frontendEndpointBuilder.GetEndpointType();
            }

            BrokerParametersBuilder& SetFrontendEndpointBaseAddress(
                const std::string& _frontendEndpointBaseAddress )
            {
                frontendEndpointBuilder.SetBaseAddress( _frontendEndpointBaseAddress );
                return *this;
            }

            const std::string& GetFrontendEndpointBaseAddress() const
            {
                return frontendEndpointBuilder.GetBaseAddress();
            }

            BrokerParametersBuilder& SetFrontendEndpointPort( int _frontendEndpointPort )
            {
                frontendEndpointBuilder.SetPort( _frontendEndpointPort );
                return *this;
            }

            int GetFrontendEndpointPort() const
            {
                return frontendEndpointBuilder.GetPort();
            }


            BrokerParametersBuilder& SetBackendEndpointType( EndpointType _backendEndpointType )
            {
                backendEndpointBuilder.SetEndpointType( _backendEndpointType );
                return *this;
            }

            EndpointType GetBackendEndpointType() const
            {
                return backendEndpointBuilder.GetEndpointType();
            }

            BrokerParametersBuilder& SetBackendEndpointBaseAddress(
                const std::string& _backendEndpointBaseAddress )
            {
                backendEndpointBuilder.SetBaseAddress( _backendEndpointBaseAddress );
                return *this;
            }

            const std::string& GetBackendEndpointBaseAddress() const
            {
                return backendEndpointBuilder.GetBaseAddress();
            }

            BrokerParametersBuilder& SetBackendEndpointPort( int _backendEndpointPort )
            {
                backendEndpointBuilder.SetPort( _backendEndpointPort );
                return *this;
            }

            int GetBackendEndpointPort() const
            {
                return backendEndpointBuilder.GetPort();
            }


            BrokerParametersBuilder& SetControlEndpointBaseAddress(
                const std::string& _controlEndpointBaseAddress )
            {
                controlEndpointBuilder.SetBaseAddress( _controlEndpointBaseAddress );
                return *this;
            }

            const std::string& GetControlEndpointBaseAddress() const
            {
                return controlEndpointBuilder.GetBaseAddress();
            }


            BrokerParametersBuilder& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }

            BrokerParametersBuilder& SetMinimumWorkerHeartbeatDelay(
                Delay _minimumWorkerHeartbeatDelay )
            {
                minimumWorkerHeartbeatDelay = _minimumWorkerHeartbeatDelay;
                return *this;
            }

            Delay GetMinimumWorkerHeartbeatDelay() const
            {
                return minimumWorkerHeartbeatDelay;
            }

            BrokerParametersBuilder& SetWorkerPulseDelay( Delay _workerPulseDelay )
            {
                workerPulseDelay = _workerPulseDelay;
                return *this;
            }

            Delay GetWorkerPulseDelay() const
            {
                return workerPulseDelay;
            }


        private:
            EndpointBuilder frontendEndpointBuilder;
            EndpointBuilder backendEndpointBuilder;
            EndpointBuilder controlEndpointBuilder;
            Delay selfHeartbeatDelay;
            Delay minimumWorkerHeartbeatDelay;
            Delay workerPulseDelay;
    };
}

#endif
