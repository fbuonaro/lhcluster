#include <lhcluster/endpointbuilder.h>

namespace LHClusterNS
{
    namespace
    {
        const char* endpointTypeSuffix( EndpointType endpointType )
        {
            switch( endpointType )
            {
                case EndpointType::InterThread:
                    return ".zitc";
                case EndpointType::InterProcess:
                    return ".zipc";
                default:
                    return "";
            }
        }
    }

    EndpointBuilder::EndpointBuilder()
    :   endpointType( EndpointType::None )
    ,   baseAddress()
    ,   port( 0 )
    ,   endpoint()
    ,   hasComponents( true )
    {
    }

    EndpointBuilder::~EndpointBuilder()
    {
    }

    bool EndpointBuilder::OK() const
    {
        if( hasComponents )
        {
            switch( endpointType )
            {
                case EndpointType::TCP:
                {
                    if( port <= 0 )
                        throw ClusterParametersBuildFailed( "invalid port" );
                }
                break;
                case EndpointType::InterThread:
                case EndpointType::InterProcess:
                {
                }
                break;
                default:
                {
                    throw ClusterParametersBuildFailed( "invalid endpoint type" );
                }
                break;
            }

            if( baseAddress.size() <= 0 )
                throw ClusterParametersBuildFailed( "invalid endpoint address" );
        }
        // else whatever is passed in should be okay, up to caller

        return true;
    }

    Endpoint EndpointBuilder::Build( std::ostringstream& oss ) const
    {
        if( hasComponents )
            return Build( oss, GetInstance(), GetProcess(), GetThread() );
        else
            return endpoint;
    }

    Endpoint EndpointBuilder::Build(
        int _instance, int _process, int _thread ) const
    {
        std::ostringstream oss;
        return Build( oss, _instance, _process, _thread );
    }

    Endpoint EndpointBuilder::Build(
        std::ostringstream& oss, int _instance, int _process, int _thread ) const
    {
        // assert hasComponents
        oss.str( std::string() );

        oss << baseAddress;
        switch( endpointType )
        {
            case EndpointType::TCP:
            {
                oss << ":" << port;
            }
            break;
            case EndpointType::InterThread:
            case EndpointType::InterProcess:
            {
                if( _instance && _process && _thread )
                    oss << "_" << _instance
                        << "_" << _process
                        << "_" << _thread;
                oss << endpointTypeSuffix( endpointType );
            }
            break;
        }

        return Endpoint( endpointType, oss.str().c_str() );
    }
}
