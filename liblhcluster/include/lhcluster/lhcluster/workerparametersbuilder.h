#ifndef __LHCLUSTER_WORKERPARAMETERSBUILDER_H__
#define __LHCLUSTER_WORKERPARAMETERSBUILDER_H__

#include <lhcluster/clusterparametersbuilder.h>
#include <lhcluster/endpointbuilder.h>
#include <lhcluster/workerparameters.h>

namespace LHClusterNS
{
    class WorkerParametersBuilder
    :   public ClusterParametersBuilder< WorkerParametersBuilder, WorkerParameters >
    {
        public:
            using ClusterParametersBuilder<
                WorkerParametersBuilder, WorkerParameters >::Build;

            WorkerParametersBuilder();
            ~WorkerParametersBuilder();

            WorkerParameters Build( std::ostringstream& ) const;
            bool OK() const;

            WorkerParametersBuilder& SetSelfMessagingEndpointPort( int port )
            {
                selfMessagingEndpointBuilder.SetPort( port );
                return *this;
            }

            int GetSelfMessagingEndpointPort() const
            {
                return selfMessagingEndpointBuilder.GetPort();
            }

            WorkerParametersBuilder& SetSelfMessagingEndpointBaseAddress(
                const std::string& baseAddress )
            {
                selfMessagingEndpointBuilder.SetBaseAddress( baseAddress );
                return *this;
            }

            const std::string& GetSelfMessagingEndpointBaseAddress() const
            {
                return selfMessagingEndpointBuilder.GetBaseAddress();
            }

            WorkerParametersBuilder& SetSelfMessagingEndpointType( EndpointType type )
            {
                selfMessagingEndpointBuilder.SetEndpointType( type );
                return *this;
            }

            EndpointType GetSelfMessagingEndpointType() const
            {
                return selfMessagingEndpointBuilder.GetEndpointType();
            }

            WorkerParametersBuilder& SetBrokerMessagingEndpoint( const Endpoint& endpoint )
            {
                brokerMessagingEndpointBuilder.SetEndpoint( endpoint );
                return *this;
            }

            const Endpoint& GetBrokerMessagingEndpoint() const
            {
                return brokerMessagingEndpointBuilder.GetEndpoint();
            }

            WorkerParametersBuilder& SetSelfControllerEndpointBaseAddress(
                const std::string& baseAddress )
            {
                selfControllerEndpointBuilder.SetBaseAddress( baseAddress );
                return *this;
            }

            const std::string& GetSelfControllerEndpointBaseAddress() const
            {
                return selfControllerEndpointBuilder.GetBaseAddress();
            }

            WorkerParametersBuilder& SetSelfInternalEndpointPort( int port )
            {
                selfInternalEndpointBuilder.SetPort( port );
                return *this;
            }

            int GetSelfInternalEndpointPort() const
            {
                return selfInternalEndpointBuilder.GetPort();
            }

            WorkerParametersBuilder& SetSelfInternalEndpointBaseAddress(
                const std::string& baseAddress )
            {
                selfInternalEndpointBuilder.SetBaseAddress( baseAddress );
                return *this;
            }

            const std::string& GetSelfInternalEndpointBaseAddress() const
            {
                return selfInternalEndpointBuilder.GetBaseAddress();
            }

            WorkerParametersBuilder& SetSelfInternalEndpointType( EndpointType type )
            {
                selfInternalEndpointBuilder.SetEndpointType( type );
                return *this;
            }

            EndpointType GetSelfInternalEndpointType() const
            {
                return selfInternalEndpointBuilder.GetEndpointType();
            }

            WorkerParametersBuilder& SetRequestHandlerControllerEndpointBaseAddress(
                const std::string& baseAddress )
            {
                requestHandlerControllerEndpointBuilder.SetBaseAddress( baseAddress );
                return *this;
            }

            const std::string& GetRequestHandlerControllerEndpointBaseAddress() const
            {
                return requestHandlerControllerEndpointBuilder.GetBaseAddress();
            }

            WorkerParametersBuilder& SetRequestHandlerInternalEndpointPort( int port )
            {
                requestHandlerInternalEndpointBuilder.SetPort( port );
                return *this;
            }

            int GetRequestHandlerInternalEndpointPort() const
            {
                return requestHandlerInternalEndpointBuilder.GetPort();
            }

            WorkerParametersBuilder& SetRequestHandlerInternalEndpointBaseAddress(
                const std::string& baseAddress )
            {
                requestHandlerInternalEndpointBuilder.SetBaseAddress( baseAddress );
                return *this;
            }

            const std::string& GetRequestHandlerInternalEndpointBaseAddress() const
            {
                return requestHandlerInternalEndpointBuilder.GetBaseAddress();
            }

            WorkerParametersBuilder& SetRequestHandlerInternalEndpointType( EndpointType type )
            {
                requestHandlerInternalEndpointBuilder.SetEndpointType( type );
                return *this;
            }

            EndpointType GetRequestHandlerInternalEndpointType() const
            {
                return requestHandlerInternalEndpointBuilder.GetEndpointType();
            }

            WorkerParametersBuilder& SetSelfHeartbeatDelay( Delay _selfHeartbeatDelay )
            {
                selfHeartbeatDelay = _selfHeartbeatDelay;
                return *this;
            }

            Delay GetSelfHeartbeatDelay() const
            {
                return selfHeartbeatDelay;
            }

            WorkerParametersBuilder& SetInitialBrokerHeartbeatDelay(
                Delay _initialBrokerHeartbeatDelay )
            {
                initialBrokerHeartbeatDelay = _initialBrokerHeartbeatDelay;
                return *this;
            }

            Delay GetInitialBrokerHeartbeatDelay() const
            {
                return initialBrokerHeartbeatDelay;
            }

            WorkerParametersBuilder& SetBrokerPulseDelay( Delay _brokerPulseDelay )
            {
                brokerPulseDelay = _brokerPulseDelay;
                return *this;
            }

            Delay GetBrokerPulseDelay() const
            {
                return brokerPulseDelay;
            }

        private:
            EndpointBuilder selfMessagingEndpointBuilder;
            EndpointBuilder brokerMessagingEndpointBuilder;
            EndpointBuilder selfControllerEndpointBuilder;
            EndpointBuilder selfInternalEndpointBuilder;
            EndpointBuilder requestHandlerInternalEndpointBuilder;
            EndpointBuilder requestHandlerControllerEndpointBuilder;
            Delay selfHeartbeatDelay;
            Delay initialBrokerHeartbeatDelay;
            Delay brokerPulseDelay;


    };
}

#endif
