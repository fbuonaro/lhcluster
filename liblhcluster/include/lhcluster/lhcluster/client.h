#ifndef __LHCLUSTER_CLIENT_H__
#define __LHCLUSTER_CLIENT_H__

#include <lhcluster/cluster.h>
#include <lhcluster/iclient.h>
#include <lhcluster/requestresponse.h>

#include <algorithm>
#include <set>

namespace LHClusterNS
{
    class ClientFactory;

    class Client : public IClient
    {
        friend ClientFactory;

        public:
            // CTORS, DTORS
            ~Client()
            {
            }

            Client( Client&& other )
            :   IClient( std::move( other ) )
            ,   iclient( std::move( other.iclient ) )
            {
            }

            Client& operator=( Client other )
            {
                swap( *this, other );
                return *this;
            }

            //METHODS
            friend void swap( Client& a, Client& b )
            {
                using std::swap;

                swap( a.iclient, b.iclient );
            }


            int Activate()
            {
                return iclient->Activate();
            }

            int Deactivate()
            {
                return iclient->Deactivate();
            }

            //Full non-blocking
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit )
            {
                return iclient->RetrieveResponses( requestResponses, responseLimit );
            }

            //Block for timeout_s seconds
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::seconds timeout_s )
            {
                return iclient->RetrieveResponses( requestResponses, responseLimit, timeout_s );
            }

            //Block for timeout_ms seconds
            int RetrieveResponses(
                std::vector< RequestResponse >&
                    requestResponses,
                int responseLimit,
                std::chrono::milliseconds timeout_ms )
            {
                return iclient->RetrieveResponses( requestResponses, responseLimit, timeout_ms );
            }

            int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId )
            {
                return iclient->SendAsynchronousRequest( requestType, requestId );
            }

            int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId )
            {
                return iclient->SendSynchronousRequest( requestType, requestId );
            }

            int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags )
            {
                return iclient->SendRequest( requestType, requestId, messageFlags );
            }

            int SendAsynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage )
            {
                return iclient->SendAsynchronousRequest( requestType, requestId, lpZMQMessage );
            }

            int SendSynchronousRequest(
                RequestType requestType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage )
            {
                return iclient->SendSynchronousRequest( requestType, requestId, lpZMQMessage );
            }

            int SendRequest(
                RequestType requestType, 
                RequestId requestId, 
                MessageFlags messageFlags,
                ZMQMessage** lpZMQMessage )
            {
                return iclient->SendRequest( requestType, requestId, messageFlags, lpZMQMessage );
            }

        private:
            // CTORS
            Client( std::unique_ptr< IClient > _iclient )
            :   IClient()
            ,   iclient( std::move( _iclient ) )
            {
            }

            // MEMBERS
            std::unique_ptr< IClient > iclient;
    };
}

#endif
