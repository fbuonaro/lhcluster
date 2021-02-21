#include <lhcluster/workerparametersbuilder.h>

namespace LHClusterNS
{
    WorkerParametersBuilder::WorkerParametersBuilder()
    :   selfMessagingEndpointBuilder()
    ,   brokerMessagingEndpointBuilder()
    ,   selfControllerEndpointBuilder()
    ,   selfInternalEndpointBuilder()
    ,   requestHandlerInternalEndpointBuilder()
    ,   requestHandlerControllerEndpointBuilder()
    ,   selfHeartbeatDelay( 0 )
    ,   initialBrokerHeartbeatDelay( 0 )
    ,   brokerPulseDelay( 0 )
    {
        selfControllerEndpointBuilder.SetEndpointType( EndpointType::InterThread );
        requestHandlerControllerEndpointBuilder.SetEndpointType( EndpointType::InterThread );
    }

    WorkerParametersBuilder::~WorkerParametersBuilder()
    {
    }

    bool WorkerParametersBuilder::OK() const
    {
        if( selfHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid self heartbeat delay" );
        if( initialBrokerHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid initial broker heartbeat delay" );
        if( brokerPulseDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid broker pulse delay" );

        return selfMessagingEndpointBuilder.OK() &&
               brokerMessagingEndpointBuilder.OK() &&
               selfControllerEndpointBuilder.OK() &&
               selfInternalEndpointBuilder.OK() &&
               requestHandlerInternalEndpointBuilder.OK() &&
               requestHandlerControllerEndpointBuilder.OK();
    }

    WorkerParameters WorkerParametersBuilder::Build( std::ostringstream& oss ) const
    {
        WorkerParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetSelfMessagingEndpoint(
            selfMessagingEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetBrokerMessagingEndpoint(
            brokerMessagingEndpointBuilder.Build( oss ) )
        .SetSelfControllerEndpoint(
            selfControllerEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetSelfInternalEndpoint(
            selfInternalEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetRequestHandlerControllerEndpoint(
            requestHandlerControllerEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetRequestHandlerInternalEndpoint(
            requestHandlerInternalEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetSelfHeartbeatDelay(
            selfHeartbeatDelay )
        .SetInitialBrokerHeartbeatDelay(
            initialBrokerHeartbeatDelay )
        .SetBrokerPulseDelay(
            brokerPulseDelay );

        return p;
    }
}
