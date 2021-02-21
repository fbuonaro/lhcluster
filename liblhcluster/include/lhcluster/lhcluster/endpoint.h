#ifndef __LHCLUSTER_ENDPOINT_H__
#define __LHCLUSTER_ENDPOINT_H__

#include <cstring>
#include <cstdint>
#include <iostream>

#define MAX_ENDPOINT_LENGTH 512

namespace LHClusterNS
{
    enum class EndpointType : std::uint8_t
    {
        None,
        TCP,
        InterThread,
        InterProcess
    };

    class Endpoint
    {
        public:
            // CTORS, DTORS
            Endpoint();
            Endpoint( EndpointType endpoint_type, const char* endpoint_path );

            // METHODS
            inline const char* path() const
            {
                return endpoint_path;
            }

            inline EndpointType type() const
            {
                return endpoint_type;
            }

            inline bool operator==( const Endpoint& e2 ) const
            {
                return( strncmp( endpoint_path, e2.endpoint_path, MAX_ENDPOINT_LENGTH ) == 0 );
            }

            friend inline std::ostream& operator<<( std::ostream& os, const Endpoint& e );

        private:
            // DATA MEMBERS
            char endpoint_path[ MAX_ENDPOINT_LENGTH + 1 ];
            EndpointType endpoint_type;
    };

    inline std::ostream& operator<<( std::ostream& os, const Endpoint& e )
    {
        return os << e.endpoint_path;
    }
}
#endif
