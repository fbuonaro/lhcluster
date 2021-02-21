#include <lhcluster/endpoint.h>

#include <stdexcept>
#include <cstdio>

#define MAX_ENDPOINT_PATH_LENGTH 504

namespace
{
    static const char* EndpointTypePrefix[] =
    {
        "None://",
        "tcp://",
        "inproc://",
        "ipc://"
    };
}

namespace LHClusterNS
{
    Endpoint::Endpoint( EndpointType _endpoint_type, const char* _endpoint_path )
    :   endpoint_type( _endpoint_type )
    {
        if( _endpoint_path )
        {
            auto len = strlen( _endpoint_path );
            if( len > MAX_ENDPOINT_PATH_LENGTH )
                throw std::runtime_error( "path to long" );
            snprintf( endpoint_path, MAX_ENDPOINT_LENGTH, "%s%s", 
                      EndpointTypePrefix[ static_cast<std::uint8_t>( _endpoint_type ) ], 
                      const_cast<char*>( _endpoint_path ) );
        }
        else
            endpoint_path[ 0 ] = '\0';
    }

    Endpoint::Endpoint()
    {
        endpoint_path[0] = '\0';
        endpoint_type = EndpointType::None;
    }
}
