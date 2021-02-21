#include <lhcluster/notifypublisherparametersbuilder.h>

namespace LHClusterNS
{
    NotifyPublisherParametersBuilder::NotifyPublisherParametersBuilder()
    :   proxyEndpointBuilder()
    ,   selfEndpointBuilder()
    {
    }

    NotifyPublisherParametersBuilder::~NotifyPublisherParametersBuilder()
    {
    }

    bool NotifyPublisherParametersBuilder::OK() const
    {

        if( ( proxyEndpointBuilder.GetEndpoint().type() == EndpointType::None ) &&
            ( selfEndpointBuilder.GetEndpointType() == EndpointType::None ) )
            throw ClusterParametersBuildFailed(
                "at least one of the proxy or the self endpoints must be set" );

        return
            ( ( ( proxyEndpointBuilder.GetEndpoint().type() != EndpointType::None ) &&
                ( proxyEndpointBuilder.OK() ) ) ||
              ( ( selfEndpointBuilder.GetEndpointType() != EndpointType::None ) &&
                ( selfEndpointBuilder.OK() ) ) );
    }

    NotifyPublisherParameters NotifyPublisherParametersBuilder::Build(
        std::ostringstream& oss ) const
    {
        NotifyPublisherParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        if( proxyEndpointBuilder.GetEndpoint().type() != EndpointType::None )
            p
            .SetProxyEndpoint(
                proxyEndpointBuilder.Build( oss ) );

        if( selfEndpointBuilder.GetEndpointType() != EndpointType::None )
            p
            .SetSelfEndpoint(
                selfEndpointBuilder.Build(
                    oss, GetInstance(), GetProcess(), GetThread() ) );

        return p;
    }
}
