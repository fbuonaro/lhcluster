#include <lhcluster/clientproxyparametersbuilder.h>

namespace LHClusterNS
{
    ClientProxyParametersBuilder::ClientProxyParametersBuilder()
    :   selfBrokerMessagingEndpointBuilder()
    ,   brokerEndpointBuilder()
    ,   selfClientMessagingEndpointBuilder()
    ,   controlEndpointBuilder()
    ,   selfHeartbeatDelay( 0 )
    ,   initialBrokerHeartbeatDelay( 0 )
    ,   brokerPulseDelay( 0 )
    {
        controlEndpointBuilder.SetEndpointType( EndpointType::InterThread );
    }

    ClientProxyParametersBuilder::~ClientProxyParametersBuilder()
    {
    }

    bool ClientProxyParametersBuilder::OK() const
    {
        if( selfHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid self heartbeat delay" );
        if( initialBrokerHeartbeatDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid initial broker heartbeat delay" );
        if( brokerPulseDelay.count() <= 0 )
            throw ClusterParametersBuildFailed( "invalid broker pulse delay" );

        return selfBrokerMessagingEndpointBuilder.OK() &&
               brokerEndpointBuilder.OK() &&
               selfClientMessagingEndpointBuilder.OK() &&
               controlEndpointBuilder.OK();
    }

    ClientProxyParameters ClientProxyParametersBuilder::Build( std::ostringstream& oss ) const
    {
        ClientProxyParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetSelfBrokerMessagingEndpoint(
            selfBrokerMessagingEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetBrokerMessagingEndpoint(
            brokerEndpointBuilder.Build( oss ) )
        .SetSelfControllerEndpoint(
            controlEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetSelfClientMessagingEndpoint(
            selfClientMessagingEndpointBuilder.Build(
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
