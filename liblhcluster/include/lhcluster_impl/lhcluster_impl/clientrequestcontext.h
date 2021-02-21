#ifndef __LHCLUSTER_IMPL_CLIENTREQUESTCONTEXT_H__
#define __LHCLUSTER_IMPL_CLIENTREQUESTCONTEXT_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/zmqaddin.h>

namespace LHClusterNS
{
namespace Impl
{

    class ClientRequestContext
    {
        private:
            RequestType type;
            RequestId id;
            LHCVersionFlags vfs;
            ClientEnvelope envelope;

        public:
            ClientRequestContext( const ClientRequestContext& ) = delete;
            ClientRequestContext& operator=( const ClientRequestContext& ) = delete;
            ClientRequestContext& operator=( ClientRequestContext&& ) = delete;

            // This class owns all of the memory that it has pointers to
            // the owner should be able to remove and take ownership of the pointers
            ClientRequestContext();
            ClientRequestContext( RequestType _type, RequestId _id );
            ClientRequestContext( RequestType requestType, 
                                  RequestId requestId, 
                                  const LHCVersionFlags& _vfs,
                                  ClientEnvelope&& envelope );
            ClientRequestContext( ClientRequestContext&& wrc );
            ~ClientRequestContext();

            bool operator<( const ClientRequestContext& other ) const;
            bool operator==( const ClientRequestContext& other ) const;
            void UpdateEnvelope( ClientRequestContext& other );

            RequestType GetRequestType() const
            {
                return type;
            }

            RequestId GetRequestId() const
            {
                return id;
            }

            void SetRequestType( RequestType _type )
            {
                type = _type;
            }

            void SetRequestId( RequestId _id )
            {
                id = _id;
            }

            void SetVersionMessageFlags(
                const LHCVersionFlags& _vfs )
            {
                vfs = _vfs;
            }

            const LHCVersionFlags&
            GetVersionMessageFlags() const
            {
                return vfs;
            }

            const ClientEnvelope& GetEnvelope() const
            {
                return envelope;
            }

            ClientEnvelope& GetEnvelope()
            {
                return envelope;
            }

            void SetEnvelope( ClientEnvelope&& _envelope )
            {
                envelope = _envelope;
            }
    };
}
}

#endif
