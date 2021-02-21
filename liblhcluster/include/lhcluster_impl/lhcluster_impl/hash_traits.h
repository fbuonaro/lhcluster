#ifndef __LHCLUSTER_IMPL_HASH_TRAITS_H__
#define __LHCLUSTER_IMPL_HASH_TRAITS_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/zmqaddin.h>

#include <functional>

namespace std
{
    template<>
    struct hash< const LHClusterNS::ZMQFrame* >
    {
        std::size_t operator()( const LHClusterNS::ZMQFrame* const &frame ) const
        {
            size_t hash = 5381;
            unsigned char* data_end = NULL;
            unsigned char* data = NULL;
            size_t frame_size =
                zframe_size(
                    const_cast< LHClusterNS::ZMQFrame* >( frame ) );

            data = zframe_data(
                const_cast< LHClusterNS::ZMQFrame* >( frame ) );
            data_end = data + frame_size;

            for( ; data != data_end; ++data )
            {
                hash = ( ( hash << 5 ) + hash ) + ( *data );
            }
            
            return hash;
        }
    };

    template<>
    struct hash< const LHClusterNS::RequestType >
    {
        std::size_t operator()( const LHClusterNS::RequestType &request_type ) const
        {
            return hash< std::uint32_t >()(
                static_cast< std::uint32_t >( request_type ) );
        }
    };
}


#endif
