#include <lhcluster_impl/clientrequestbuffer.h>

namespace LHClusterNS
{
namespace Impl
{
    ClientRequestBuffer::ClientRequestBuffer()
    :   bufferedRequests( )
    {
    }

    ClientRequestBuffer::ClientRequestBuffer( ClientRequestBuffer&& buffer )
    :   bufferedRequests(
            std::move< std::map< ClientRequestBufferKey, ClientRequestBufferValue >& >( 
                buffer.bufferedRequests ) )
    {
    }

    ClientRequestBuffer::~ClientRequestBuffer()
    {
        for( auto it = bufferedRequests.begin();
             it != bufferedRequests.end();
             ++it )
        {
            zmsg_destroy( &std::get< 2 >( it->second ) );
        }
    }

    //Assume that the identity is apart of request_msg
    void ClientRequestBuffer::Add(
        RequestType requestType,
        RequestId requestId,
        ZMQFrame* identity,
        const LHCVersionFlags& vfs,
        ZMQMessage* request_msg )
    {
        auto ret(
            bufferedRequests.emplace( 
                    ClientRequestBufferKey(
                        requestType,
                        requestId,
                        identity ),
                    ClientRequestBufferValue() ) );

        if( !( ret.second ) )
            zmsg_destroy( &std::get< 2 >( ret.first->second ) );

        ret.first->second = ClientRequestBufferValue( vfs, vfs.flags, request_msg );
    }

    void ClientRequestBuffer::EraseIfLastReceipt(
        ClusterMessage receiptType,
        RequestType requestType,
        RequestId requestId,
        ZMQFrame* identity )
    {
        auto it = bufferedRequests.find( 
            ClientRequestBufferKey( requestType, requestId, identity ) );
        if( it != bufferedRequests.end() )
        {
            MessageFlags& currentFlags( std::get< 1 >( it->second ) );

            switch( receiptType )
            {
                case ClusterMessage::BrokerReceivedReceipt:
                {
                    currentFlags &= ~MessageFlag::ReceiptOnReceive;
                    break;
                }
                case ClusterMessage::BrokerSentReceipt:
                {
                    currentFlags &= ~MessageFlag::ReceiptOnSend;
                    break;
                }
                default:
                {
                    break;
                }
            }

            if( !( currentFlags&( MessageFlag::ReceiptOnSend | MessageFlag::ReceiptOnReceive ) ) )
            {
                zmsg_destroy( &std::get< 2 >( it->second ) );
                bufferedRequests.erase( it );
            }
        }
    }

    void ClientRequestBuffer::Erase(
        RequestType requestType,
        RequestId requestId,
        ZMQFrame* identity )
    {
        auto it = bufferedRequests.find( 
            ClientRequestBufferKey( requestType, requestId, identity ) );
        if( it != bufferedRequests.end() )
        {
            zmsg_destroy( &std::get< 2 >( it->second ) );
            bufferedRequests.erase( it );
        }
    }
}
}
