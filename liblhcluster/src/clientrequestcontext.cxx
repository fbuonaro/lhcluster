#include <lhcluster_impl/clientrequestcontext.h>

namespace LHClusterNS
{
namespace Impl
{
    using namespace std;

    ClientRequestContext::ClientRequestContext( RequestType _type, RequestId _id )
    :   type ( _type )
    ,   id( _id )
    ,   vfs()
    ,   envelope()
    {
    }

    ClientRequestContext::ClientRequestContext( )
    :   type ( 0 )
    ,   id( 0 )
    ,   vfs()
    ,   envelope()
    {
    }

    ClientRequestContext::ClientRequestContext(
        RequestType requestType, 
        RequestId requestId, 
        const LHCVersionFlags& _vfs,
        ClientEnvelope&& envelope )
    :   type ( requestType )
    ,   id( requestId )
    ,   vfs( _vfs )
    ,   envelope( std::forward< ClientEnvelope >( envelope ) )
    {
    }

    ClientRequestContext::ClientRequestContext( ClientRequestContext&& wrc )
    :   type ( wrc.type )
    ,   id( wrc.id )
    ,   vfs ( wrc.vfs )
    ,   envelope( std::move( wrc.envelope ) )
    {
        wrc.id = 0;
        wrc.type = 0;
        wrc.vfs.Reset();
    }

    ClientRequestContext::~ClientRequestContext( )
    {
        for( auto it = envelope.begin(); it != envelope.end(); ++it )
        {
            ZMQFrame* frame = *it;
            zframe_destroy( &frame );
        }
    }

    bool ClientRequestContext::operator==( const ClientRequestContext& other ) const
    {
        return  ( ( type == other.type ) && ( id == other.id ) );
    }

    bool ClientRequestContext::operator<( const ClientRequestContext& other ) const
    {
        return  ( type < other.type )
                ?   true
                :   ( type == other.type )
                    ?   ( id < other.id )
                    :   false;
    }

    void ClientRequestContext::UpdateEnvelope( ClientRequestContext& other )
    {
        for( auto it = envelope.begin(); it != envelope.end(); ++it )
        {
            ZMQFrame* frame = *it;
            zframe_destroy( &frame );
        }

        envelope = std::move( other.envelope );
    }
}
}
