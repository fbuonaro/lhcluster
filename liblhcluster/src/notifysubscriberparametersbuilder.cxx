#include <lhcluster/notifysubscriberparametersbuilder.h>

namespace LHClusterNS
{
    NotifySubscriberParametersBuilder::NotifySubscriberParametersBuilder()
    :   selfControllerEndpointBuilder()
    ,   publisherEndpointBuilder()
    {
        selfControllerEndpointBuilder.SetEndpointType( EndpointType::InterThread );
    }

    NotifySubscriberParametersBuilder::~NotifySubscriberParametersBuilder()
    {
    }

    bool NotifySubscriberParametersBuilder::OK() const
    {
        return selfControllerEndpointBuilder.OK() && publisherEndpointBuilder.OK();
    }


    NotifySubscriberParameters NotifySubscriberParametersBuilder::Build(
        std::ostringstream& oss ) const
    {
        NotifySubscriberParameters p;

        if( !OK() )
            return p;

        oss.str( std::string() );

        p
        .SetSelfControllerEndpoint(
            selfControllerEndpointBuilder.Build(
                oss, GetInstance(), GetProcess(), GetThread() ) )
        .SetPublisherEndpoint(
            publisherEndpointBuilder.Build( oss ) );

        return p;
    }
}
