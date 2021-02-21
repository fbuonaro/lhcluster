#include <lhcluster/clientparametersbuilder.h>

namespace LHClusterNS
{
    ClientParametersBuilder::ClientParametersBuilder()
    :   ClusterParametersBuilder()
    ,   clientEndpointBuilder()
    ,   clientProxyEndpointBuilder()
    {
    }

    ClientParametersBuilder::~ClientParametersBuilder()
    {
    }

    bool ClientParametersBuilder::OK() const
    {
        return clientEndpointBuilder.OK() && clientProxyEndpointBuilder.OK();
    }

    ClientParameters ClientParametersBuilder::Build( std::ostringstream& oss ) const
    {
        ClientParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetClientMessagingEndpoint(
            clientEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetClientProxyMessagingEndpoint(
            clientProxyEndpointBuilder.Build( oss ) );

        return p;
    }

}
