#include <lhcluster/requestresponse.h>

#include <lhcluster_impl/utils.h>
#include <lhcluster_impl/zmqframehandler.h>

namespace LHClusterNS
{
    BadRequestResponse::BadRequestResponse( const char* _errorMessage )
    {
        strncpy( errorMessage, _errorMessage, sizeof( errorMessage ) );
        errorMessage[ sizeof( errorMessage ) - 1 ] = '\0';
    }

    const char* BadRequestResponse::what() const throw()
    {
        return errorMessage;
    }

    RequestResponse::RequestResponse()
    :   id( 0 )
    ,   type( 0 )
    ,   status( RequestStatus::None )
    ,   message( nullptr )
    ,   vfs()
    {
    }

    RequestResponse::RequestResponse(
        RequestId _id,
        RequestType _type,
        RequestStatus _status,
        ZMQMessage** lpMessage,
        const LHCVersionFlags& _vfs )
    :   id( _id )
    ,   type( _type )
    ,   status( _status )
    ,   message( *lpMessage )
    ,   vfs( _vfs )
    {
        *lpMessage = nullptr;
    }
    
    // expects fullResponse to encompass the entire response as received from the ClientProxy:
    //  NULL | RequestType | RequestId | RequestStatus
    //  [ | zero or more additional frames for the worker message ]
    // this will destroy thr response before it is done.
    // If it throws, the fullResponse must be cleaned up by
    // the caller
    RequestResponse::RequestResponse( ZMQMessage** lpFullResponse )
    {
        Impl::ZMQFrameHandler frameHandler;
        ZMQFrame* frame = nullptr;
        ZMQMessage* fullResponse = *lpFullResponse;
    
        frame = zmsg_pop( fullResponse );
        if( !frame || zframe_size( frame ) )
        {
            throw BadRequestResponse( "empty or malformed response" );
        }
        zframe_destroy( &frame );
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing version and flags" );
        }
        vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
        zframe_destroy( &frame );

        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing type" );
        }
        type = frameHandler.get_frame_value< RequestType >( frame );
        zframe_destroy( &frame );
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing id" );
        }
        id = frameHandler.get_frame_value< RequestId >( frame );
        zframe_destroy( &frame );
    
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing status" );
        }
        status = frameHandler.get_frame_value< RequestStatus >( frame );
        zframe_destroy( &frame );
    
        message = fullResponse;
        *lpFullResponse = nullptr;
    }
    
    RequestResponse::RequestResponse( const RequestResponse& other )
    :   id( other.id )
    ,   type( other.type )
    ,   status( other.status )
    ,   message( zmsg_dup( other.message ) )
    ,   vfs( other.vfs )
    {
    }
     RequestResponse::RequestResponse( RequestResponse&& other )
    :   id( other.id )
    ,   type( other.type )
    ,   status( other.status )
    ,   message( other.message )
    ,   vfs( other.vfs )
    {
        other.message = nullptr;
    }
    
    bool RequestResponse::operator==( const RequestResponse& other )
    {
        return ( ( id == other.id ) &&
                 ( type == other.type ) &&
                 ( status == other.status ) &&
                 zmsg_eq( message, other.message ) &&
                 ( vfs == other.vfs ) );
    }

    RequestResponse& RequestResponse::operator=( RequestResponse other )
    {
        swap( *this, other );
        return *this;
    }
    
    RequestResponse::~RequestResponse( )
    {
        if( message )
            zmsg_destroy( &message );
    }

    void RequestResponse::FillFromMessage( ZMQMessage** lpFullResponse )
    {
        Impl::ZMQFrameHandler frameHandler;
        ZMQFrame* frame = nullptr;
        ZMQMessage* fullResponse = *lpFullResponse;
    
        frame = zmsg_pop( fullResponse );
        if( !frame || zframe_size( frame ) )
        {
            throw BadRequestResponse( "empty or malformed response" );
        }
        zframe_destroy( &frame );
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing version and flags" );
        }
        vfs = frameHandler.get_frame_value< LHCVersionFlags >( frame );
        zframe_destroy( &frame );
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing type" );
        }
        type = frameHandler.get_frame_value< RequestType >( frame );
        zframe_destroy( &frame );
    
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing id" );
        }
        id = frameHandler.get_frame_value< RequestId >( frame );
        zframe_destroy( &frame );
    
    
        frame = zmsg_pop( fullResponse );
        if( !( frame && zframe_size( frame ) ) )
        {
            throw BadRequestResponse( "missing status" );
        }
        status = frameHandler.get_frame_value< RequestStatus >( frame );
        zframe_destroy( &frame );
    
        message = fullResponse;
        *lpFullResponse = nullptr;
    }
}
