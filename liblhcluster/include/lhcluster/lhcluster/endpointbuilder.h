#ifndef __LHCLUSTER_ENDPOINTBUILDER_H__
#define __LHCLUSTER_ENDPOINTBUILDER_H__

#include <lhcluster/endpoint.h>
#include <lhcluster/clusterparametersbuilder.h>

#include <string>
#include <sstream>

namespace LHClusterNS
{
    class EndpointBuilder
    :   public ClusterParametersBuilder< EndpointBuilder, Endpoint >
    {
        public:
            // to pick up parameterless Build from parent
            using ClusterParametersBuilder< EndpointBuilder, Endpoint >::Build;

            EndpointBuilder();
            ~EndpointBuilder();

            bool OK() const;
            Endpoint Build( std::ostringstream& oss ) const;
            Endpoint Build(
                int instance, int process, int thread ) const;
            Endpoint Build(
                std::ostringstream& oss, int instance, int process, int thread ) const;

            EndpointBuilder& SetBaseAddress( const std::string& _baseAddress )
            {
                hasComponents = true;
                baseAddress = _baseAddress;
                return *this;
            }

            const std::string& GetBaseAddress() const
            {
                // assert hasComponents
                return baseAddress;
            }

            EndpointBuilder& SetPort( int _port )
            {
                hasComponents = true;
                port = _port;
                return *this;
            }

            int GetPort() const
            {
                // assert hasComponents
                return port;
            }

            EndpointBuilder& SetEndpointType( EndpointType _endpointType )
            {
                hasComponents = true;
                endpointType = _endpointType;
                return *this;
            }

            EndpointType GetEndpointType() const
            {
                // assert hasComponents
                return endpointType;
            }

            EndpointBuilder& SetEndpoint( const Endpoint& _endpoint )
            {
                hasComponents = false;
                endpoint = _endpoint;
                return *this;
            }

            const Endpoint& GetEndpoint() const
            {
                // assert !hasComponents
                return endpoint;
            }

        private:
            // variant
            EndpointType endpointType;
            std::string baseAddress;
            int port;
            Endpoint endpoint;
            bool hasComponents;
    };
}

#endif
