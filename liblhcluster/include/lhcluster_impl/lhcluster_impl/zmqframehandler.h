#ifndef __LHCLUSTER_IMPL_ZMQFRAMEHANDLER_H__
#define __LHCLUSTER_IMPL_ZMQFRAMEHANDLER_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/zmqaddin.h>

#include "zmqframetype.pb.h"

#include <google/protobuf/message_lite.h>

#include <ctime>
#include <string>
#include <vector>

namespace LHClusterNS
{
namespace Impl
{
    class ZMQFrameHandler
    {
        public:
            // CTORS, DTORS
            ZMQFrameHandler();

            // METHODS
            template< class T >
            inline T get_frame_value( ZMQFrame* frame );

            template< class T >
            inline T get_frame_value_as_str( ZMQFrame* frame );

            template< class T >
            inline int append_message_value_as_str( ZMQMessage* msg, const T& value );

            template< class T >
            inline int prepend_message_value_as_str( ZMQMessage* msg, const T& value );

            template< class T >
            inline int append_message_value( ZMQMessage* msg, const T& value );

            template< class T >
            inline int prepend_message_value( ZMQMessage* msg, const T& value );

        private:
            // DATA MEMBERS
            // buffers ...
            ZMQFrame_time_t frameTime;
            ZMQFrame_int32_t frameInt32;
            ZMQFrame_uint32_t frameUInt32;
            ZMQFrame_uint64_t frameUInt64;
            ZMQFrame_string_t frameString;
            ZMQFrame_versionflags_t frameVersionFlags;
            std::string stringBuffer;
            std::vector< std::uint8_t > byteBuffer;

            // METHODS
            static void parseFrameIntoMessage(
                ZMQFrame* frame, google::protobuf::MessageLite& message );
    };

#undef ADD_ZMQFrameHandler_ZMQFrame_t
#define ADD_ZMQFrameHandler_ZMQFrame_t( t ) \
    template<> \
    t ZMQFrameHandler::get_frame_value< t >( ZMQFrame* frame ); \
    template<> \
    t ZMQFrameHandler::get_frame_value_as_str< t >( ZMQFrame* frame ); \
    template<> \
    int ZMQFrameHandler::append_message_value_as_str< t >( ZMQMessage* msg, const t& v ); \
    template<> \
    int ZMQFrameHandler::prepend_message_value_as_str< t >( ZMQMessage* msg, const t& v ); \
    template<> \
    int ZMQFrameHandler::append_message_value< t >( ZMQMessage* msg, const t& v ); \
    template<> \
    int ZMQFrameHandler::prepend_message_value< t >( ZMQMessage* msg, const t& v );

    ADD_ZMQFrameHandler_ZMQFrame_t( std::time_t )

    ADD_ZMQFrameHandler_ZMQFrame_t( std::int32_t )

    ADD_ZMQFrameHandler_ZMQFrame_t( std::uint32_t )

    ADD_ZMQFrameHandler_ZMQFrame_t( std::uint64_t )

    ADD_ZMQFrameHandler_ZMQFrame_t( std::string )

    template<>
    int ZMQFrameHandler::append_message_value_as_str< google::protobuf::MessageLite >(
        ZMQMessage* msg, const google::protobuf::MessageLite& value );

    template<>
    int ZMQFrameHandler::prepend_message_value_as_str< google::protobuf::MessageLite >(
        ZMQMessage* msg, const google::protobuf::MessageLite& value );

    template<>
    int ZMQFrameHandler::append_message_value< google::protobuf::MessageLite >(
        ZMQMessage* msg, const google::protobuf::MessageLite& value );

    template<>
    int ZMQFrameHandler::prepend_message_value< google::protobuf::MessageLite >(
        ZMQMessage* msg, const google::protobuf::MessageLite& value );

    ADD_ZMQFrameHandler_ZMQFrame_t( RequestStatus )

    ADD_ZMQFrameHandler_ZMQFrame_t( ClusterMessage )

    ADD_ZMQFrameHandler_ZMQFrame_t( ClientCommand )

    ADD_ZMQFrameHandler_ZMQFrame_t( WorkerCommand )

    ADD_ZMQFrameHandler_ZMQFrame_t( BrokerCommand )

    ADD_ZMQFrameHandler_ZMQFrame_t( LHCVersionFlags )

    ADD_ZMQFrameHandler_ZMQFrame_t( NotifySubscriberCommand )
}
}

#endif
