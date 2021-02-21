#ifndef __LHCLUSTER_NOTIFYBROKERPARAMETERSBUILDER_H__
#define __LHCLUSTER_NOTIFYBROKERPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/notifybrokerparameters.h>

namespace LHClusterNS
{
    class NotifyBrokerParametersBuilder
    :   public ClusterParametersBuilder< NotifyBrokerParametersBuilder, NotifyBrokerParameters >
    {
        public:
            using ClusterParametersBuilder<
                NotifyBrokerParametersBuilder, NotifyBrokerParameters >::Build;

            NotifyBrokerParametersBuilder();
            ~NotifyBrokerParametersBuilder();

            bool OK() const;
            NotifyBrokerParameters Build( std::ostringstream& oss ) const;

            NotifyBrokerParametersBuilder& SetFrontendEndpointType(
                EndpointType _frontendEndpointType )
            {
                frontendMessagingEndpointBuilder.SetEndpointType( _frontendEndpointType );
                return *this;
            }

            EndpointType GetFrontendEndpointType() const
            {
                return frontendMessagingEndpointBuilder.GetEndpointType();
            }

            NotifyBrokerParametersBuilder& SetFrontendEndpointBaseAddress(
                const std::string& _frontendEndpointBaseAddress )
            {
                frontendMessagingEndpointBuilder.SetBaseAddress( _frontendEndpointBaseAddress );
                return *this;
            }

            const std::string& GetFrontendEndpointBaseAddress() const
            {
                return frontendMessagingEndpointBuilder.GetBaseAddress();
            }

            NotifyBrokerParametersBuilder& SetFrontendEndpointPort( int _frontendEndpointPort )
            {
                frontendMessagingEndpointBuilder.SetPort( _frontendEndpointPort );
                return *this;
            }

            int GetFrontendEndpointPort() const
            {
                return frontendMessagingEndpointBuilder.GetPort();
            }

            NotifyBrokerParametersBuilder& SetBackendEndpointType(
                EndpointType _backendEndpointType )
            {
                backendMessagingEndpointBuilder.SetEndpointType( _backendEndpointType );
                return *this;
            }

            EndpointType GetBackendEndpointType() const
            {
                return backendMessagingEndpointBuilder.GetEndpointType();
            }

            NotifyBrokerParametersBuilder& SetBackendEndpointBaseAddress(
                const std::string& _backendEndpointBaseAddress )
            {
                backendMessagingEndpointBuilder.SetBaseAddress( _backendEndpointBaseAddress );
                return *this;
            }

            const std::string& GetBackendEndpointBaseAddress() const
            {
                return backendMessagingEndpointBuilder.GetBaseAddress();
            }

            NotifyBrokerParametersBuilder& SetBackendEndpointPort( int _backendEndpointPort )
            {
                backendMessagingEndpointBuilder.SetPort( _backendEndpointPort );
                return *this;
            }

            int GetBackendEndpointPort() const
            {
                return backendMessagingEndpointBuilder.GetPort();
            }


        private:
            EndpointBuilder frontendMessagingEndpointBuilder;
            EndpointBuilder backendMessagingEndpointBuilder;
    };
}

#endif
