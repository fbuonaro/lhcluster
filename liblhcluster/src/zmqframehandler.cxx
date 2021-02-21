#include <lhcluster_impl/zmqframehandler.h>

#include <google/protobuf/util/time_util.h>

#include <sstream>
#include <stdexcept>

namespace LHClusterNS
{
namespace Impl
{
    using namespace std;
    using namespace google::protobuf;
    using namespace google::protobuf::util;

    ZMQFrameHandler::ZMQFrameHandler()
    {
        frameTime.set_keep( true );
        frameInt32.set_keep( true );
        frameUInt32.set_keep( true );
        frameUInt64.set_keep( true );
        frameString.set_keep( true );
        frameVersionFlags.set_keep( true );
    }

    void ZMQFrameHandler::parseFrameIntoMessage(
        ZMQFrame* frame, MessageLite& message )
    {
        bool ret =
            message.ParseFromArray( zframe_data( frame ), zframe_size( frame ) );
        if( !ret )
        {
            string s(
                "ZMQFrameHandler.parseFrameIntoMessage:failed to parse 1 " );
            s += message.GetTypeName();
            throw runtime_error( s );
        }
    }

// for basic types
#undef ADD_ZMQFrameHandler_ZMQFrame_proto_t
#define ADD_ZMQFrameHandler_ZMQFrame_proto_t( t, b ) \
    template<> \
    t ZMQFrameHandler::get_frame_value< t >( \
        ZMQFrame* frame ) \
    { \
        b.Clear(); \
        ZMQFrameHandler::parseFrameIntoMessage( frame, b ); \
        return b.v(); \
    } \
    template<> \
    t ZMQFrameHandler::get_frame_value_as_str< t >( \
        ZMQFrame* frame ) \
    { \
        t v; \
        istringstream iss; \
        iss.exceptions( istringstream::failbit | istringstream::badbit ); \
        stringBuffer.assign( \
            ( char* ) zframe_data( frame ), \
            zframe_size( frame ) ); \
        iss.str( stringBuffer ); \
        iss >> v; \
        return v; \
    } \
    template<> \
    int ZMQFrameHandler::append_message_value_as_str< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        ostringstream oss; \
        int ret = 0; \
        oss << v; \
        if( oss.good() ) \
        { \
            zmsg_addstr( msg, oss.str().c_str() ); \
        } \
        else \
        { \
            ret = 1; \
        } \
        return ret; \
    } \
    template<> \
    int ZMQFrameHandler::prepend_message_value_as_str< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        ostringstream oss; \
        int ret = 0; \
        oss << v; \
        if( oss.good() ) \
        { \
            zmsg_pushstr( msg, oss.str().c_str() ); \
        } \
        else \
        { \
            ret = 1; \
        } \
        return ret; \
    } \
    template<> \
    int ZMQFrameHandler::prepend_message_value< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( v ); \
        return prepend_message_value< MessageLite >( msg, b ); \
    } \
    template<> \
    int ZMQFrameHandler::append_message_value< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( v ); \
        return append_message_value< MessageLite >( msg, b ); \
    }


    //time_t
    template<>
    time_t ZMQFrameHandler::get_frame_value< time_t >(
        ZMQFrame* frame )
    {
        frameTime.Clear();
        ZMQFrameHandler::parseFrameIntoMessage( frame, frameTime );
        return TimeUtil::TimestampToSeconds( frameTime.v() );
    }

    template<>
    int ZMQFrameHandler::append_message_value_as_str< time_t >(
        ZMQMessage* msg, const time_t& v )
    {
        *frameTime.mutable_v() =  TimeUtil::SecondsToTimestamp( v  );
        return append_message_value_as_str< MessageLite >( msg, frameTime );
    }

    template<>
    int ZMQFrameHandler::prepend_message_value_as_str< time_t >(
        ZMQMessage* msg, const time_t& v )
    {
        *frameTime.mutable_v() =  TimeUtil::SecondsToTimestamp( v  );
        return prepend_message_value_as_str< MessageLite >( msg, frameTime );
    }

    template<>
    int ZMQFrameHandler::prepend_message_value< time_t >(
        ZMQMessage* msg, const time_t& v )
    {
        *frameTime.mutable_v() =  TimeUtil::SecondsToTimestamp( v  );
        return prepend_message_value< MessageLite >( msg, frameTime );
    }

    template<>
    int ZMQFrameHandler::append_message_value< time_t >(
        ZMQMessage* msg, const time_t& v )
    {
        *frameTime.mutable_v() =  TimeUtil::SecondsToTimestamp( v  );
        return append_message_value< MessageLite >( msg, frameTime );
    }

    //uint32_t
    ADD_ZMQFrameHandler_ZMQFrame_proto_t( uint32_t, frameUInt32 )

    //int32_t
    ADD_ZMQFrameHandler_ZMQFrame_proto_t( int32_t, frameInt32 )

    //uint64_t
    ADD_ZMQFrameHandler_ZMQFrame_proto_t( uint64_t, frameUInt64 )

    //string
    ADD_ZMQFrameHandler_ZMQFrame_proto_t( string, frameString )

// for enums
#undef ADD_ZMQFrameHandler_ZMQFrame_proto_t
#define ADD_ZMQFrameHandler_ZMQFrame_proto_t( t, ut, b ) \
    template<> \
    t ZMQFrameHandler::get_frame_value< t >( \
        ZMQFrame* frame ) \
    { \
        b.Clear(); \
        ZMQFrameHandler::parseFrameIntoMessage( frame, b ); \
        return static_cast< t >( b.v() ); \
    } \
    template<> \
    int ZMQFrameHandler::append_message_value_as_str< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( static_cast< ut >( v ) ); \
        return append_message_value_as_str< MessageLite >( msg, b ); \
    } \
    template<> \
    int ZMQFrameHandler::prepend_message_value_as_str< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( static_cast< ut >( v ) ); \
        return prepend_message_value_as_str< MessageLite >( msg, b ); \
    } \
    template<> \
    int ZMQFrameHandler::prepend_message_value< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( static_cast< ut >( v ) ); \
        return prepend_message_value< MessageLite >( msg, b ); \
    } \
    template<> \
    int ZMQFrameHandler::append_message_value< t >( \
        ZMQMessage* msg, const t& v ) \
    { \
        b.set_v( static_cast< ut >( v ) ); \
        return append_message_value< MessageLite >( msg, b ); \
    }

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( RequestStatus, uint32_t, frameUInt32 )

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( ClusterMessage, uint32_t, frameUInt32  )

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( ClientCommand, uint32_t, frameUInt32  )

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( WorkerCommand, uint32_t, frameUInt32  )

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( BrokerCommand, uint32_t, frameUInt32  )

    ADD_ZMQFrameHandler_ZMQFrame_proto_t( NotifySubscriberCommand, uint32_t, frameUInt32  )

    template<>
    int ZMQFrameHandler::append_message_value_as_str< MessageLite >(
        ZMQMessage* msg, const MessageLite& v )
    {
        int ret = 0;
        stringBuffer.clear();
        if( v.SerializeToString( &stringBuffer ) )
        {
            // fprintf( stderr, "TEST: appending string[%s]\n", stringBuffer.c_str() );
            zmsg_addstr( msg, stringBuffer.c_str() );
        }
        else
            ret = 1;

        return ret;
    }

    template<>
    int ZMQFrameHandler::prepend_message_value_as_str< MessageLite >(
        ZMQMessage* msg, const MessageLite& v )
    {
        int ret = 0;
        stringBuffer.clear();
        if( v.SerializeToString( &stringBuffer ) )
        {
            // fprintf( stderr, "TEST: prepending string[%s]\n", stringBuffer.c_str() );
            zmsg_pushstr( msg, stringBuffer.c_str() );
        }
        else
            ret = 1;

        return ret;
    }

    template<>
    int ZMQFrameHandler::prepend_message_value< MessageLite >(
        ZMQMessage* msg, const MessageLite& v )
    {
        size_t vSize( v.ByteSizeLong() );
        int ret = 0;
        byteBuffer.clear();
        byteBuffer.resize( vSize );
        if( v.SerializeToArray( byteBuffer.data(), vSize ) )
            zmsg_pushmem( msg, byteBuffer.data(), vSize );
        else
            ret = 1;
        return ret;
    }

    template<>
    int ZMQFrameHandler::append_message_value< MessageLite >(
        ZMQMessage* msg, const MessageLite& v )
    {
        size_t vSize( v.ByteSizeLong() );
        int ret = 0;
        byteBuffer.clear();
        byteBuffer.resize( vSize );
        if( v.SerializeToArray( byteBuffer.data(), vSize ) )
            zmsg_addmem( msg, byteBuffer.data(), vSize );
        else
            ret = 1;
        return ret;
    }

    // TODO
    //VersionFlags
    template<>
    LHCVersionFlags ZMQFrameHandler::get_frame_value< LHCVersionFlags >(
        ZMQFrame* frame )
    {
        frameVersionFlags.Clear();
        ZMQFrameHandler::parseFrameIntoMessage( frame, frameVersionFlags );
        return LHCVersionFlags(
            frameVersionFlags.vmajor(), frameVersionFlags.vminor(), frameVersionFlags.vflags() );
    }

    template<>
    int ZMQFrameHandler::append_message_value_as_str< LHCVersionFlags >(
        ZMQMessage* msg, const LHCVersionFlags& v )
    {
        frameVersionFlags.set_vmajor( v.version.lhcmajor );
        frameVersionFlags.set_vminor( v.version.lhcminor );
        frameVersionFlags.set_vflags( v.flags );
        return append_message_value_as_str< MessageLite >( msg, frameVersionFlags );
    }

    template<>
    int ZMQFrameHandler::prepend_message_value_as_str< LHCVersionFlags >(
        ZMQMessage* msg, const LHCVersionFlags& v )
    {
        frameVersionFlags.set_vmajor( v.version.lhcmajor );
        frameVersionFlags.set_vminor( v.version.lhcminor );
        frameVersionFlags.set_vflags( v.flags );
        return prepend_message_value_as_str< MessageLite >( msg, frameVersionFlags );
    }

    template<>
    int ZMQFrameHandler::prepend_message_value< LHCVersionFlags >(
        ZMQMessage* msg, const LHCVersionFlags& v )
    {
        frameVersionFlags.set_vmajor( v.version.lhcmajor );
        frameVersionFlags.set_vminor( v.version.lhcminor );
        frameVersionFlags.set_vflags( v.flags );
        return prepend_message_value< MessageLite >( msg, frameVersionFlags );
    }

    template<>
    int ZMQFrameHandler::append_message_value< LHCVersionFlags >(
        ZMQMessage* msg, const LHCVersionFlags& v )
    {
        frameVersionFlags.set_vmajor( v.version.lhcmajor );
        frameVersionFlags.set_vminor( v.version.lhcminor );
        frameVersionFlags.set_vflags( v.flags );
        return append_message_value< MessageLite >( msg, frameVersionFlags );
    }
}
}
