#include <lhcluster/brokerparametersbuilder.h>

namespace LHClusterNS
{
    BrokerParametersBuilder::BrokerParametersBuilder()
    :   ClusterParametersBuilder()
    ,   frontendEndpointBuilder()
    ,   backendEndpointBuilder()
    ,   controlEndpointBuilder()
    ,   selfHeartbeatDelay( 0 )
    ,   minimumWorkerHeartbeatDelay( 0 )
    ,   workerPulseDelay( 0 )
    {
        controlEndpointBuilder.SetEndpointType( EndpointType::InterThread );
    }

    BrokerParametersBuilder::~BrokerParametersBuilder()
    {
    }

    bool BrokerParametersBuilder::OK() const
    {
        if( selfHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid self heartbeat delay" );
        if( minimumWorkerHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid min worker heartbeat delay" );
        if( workerPulseDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid worker pulse delay" );

        return frontendEndpointBuilder.OK() && 
               backendEndpointBuilder.OK() &&
               controlEndpointBuilder.OK();
    }

    BrokerParameters BrokerParametersBuilder::Build( std::ostringstream& oss ) const
    {
        BrokerParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetFrontendMessagingEndpoint(
            frontendEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetBackendMessagingEndpoint(
            backendEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetControlEndpoint(
            controlEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetSelfHeartbeatDelay( selfHeartbeatDelay )
        .SetMinimumWorkerHeartbeatDelay( minimumWorkerHeartbeatDelay )
        .SetWorkerPulseDelay( workerPulseDelay );

        return p;
    }
}
