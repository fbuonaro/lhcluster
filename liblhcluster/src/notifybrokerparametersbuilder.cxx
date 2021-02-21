#include <lhcluster/notifybrokerparametersbuilder.h>

namespace LHClusterNS
{
    NotifyBrokerParametersBuilder::NotifyBrokerParametersBuilder()
    :   frontendMessagingEndpointBuilder()
    ,   backendMessagingEndpointBuilder()
    {
    }

    NotifyBrokerParametersBuilder::~NotifyBrokerParametersBuilder()
    {
    }

    bool NotifyBrokerParametersBuilder::OK() const
    {
        return frontendMessagingEndpointBuilder.OK() && backendMessagingEndpointBuilder.OK();
    }


    NotifyBrokerParameters NotifyBrokerParametersBuilder::Build( std::ostringstream& oss ) const
    {
        NotifyBrokerParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetFrontendMessagingEndpoint(
            frontendMessagingEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetBackendMessagingEndpoint(
            backendMessagingEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) );

        return p;
    }
}
