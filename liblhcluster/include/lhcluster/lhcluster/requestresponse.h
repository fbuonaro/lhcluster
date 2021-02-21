#ifndef __LHCLUSTER_REQUESTRESPONSE_H__
#define __LHCLUSTER_REQUESTRESPONSE_H__

#include <lhcluster/cluster.h>
#include <lhcluster/zmqaddin.h>

#include <exception>
#include <utility>

namespace LHClusterNS
{
    class BadRequestResponse : public std::exception
    {
        public:
            BadRequestResponse( const char* _errorMessage );
            const char* what() const throw();

        private:
            char errorMessage[ 128 ];
    };

    class RequestResponse
    {
        public:
            // CTORS, DTORS
            RequestResponse();
            RequestResponse(
                RequestId _id,
                RequestType _type,
                RequestStatus _status,
                ZMQMessage** lpMessage,
                const LHCVersionFlags& vfs );
            // expects fullResponse to encompass the entire response as 
            // received from the ClientProxy
            //      NULL | RequestType | RequestId | RequestStatus 
            //      [ | zero or more additional frames for the worker message ]
            // this will destroy the response before it is done.
            // If it throws, the fullResponse must be cleaned up by
            // the caller
            RequestResponse( ZMQMessage** lpFullResponse );
            RequestResponse( const RequestResponse& other_response );
            RequestResponse( RequestResponse&& other_response );
            RequestResponse& operator=( RequestResponse other );
            ~RequestResponse();

            bool operator==( const RequestResponse& other );

            // METHODS
            friend void swap( RequestResponse& a, RequestResponse& b )
            {
                using std::swap;

                swap( a.vfs, b.vfs );
                swap( a.type, b.type );
                swap( a.id, b.id );
                swap( a.status, b.status );
                swap( a.message, b.message );
            }

            void FillFromMessage( ZMQMessage** lpFullResponse );

            RequestId GetRequestId() const
            {
                return id;
            }

            RequestType GetRequestType() const
            {
                return type;
            }

            RequestStatus GetRequestStatus() const
            {
                return status;
            }

            ZMQMessage* GetMessageBody() const
            {
                return message;
            }

            const LHCVersionFlags& GetVersionAndFlags() const
            {
                return vfs;
            }

        private:
            // DATA MEMBERS
            RequestId id;
            RequestType type;
            RequestStatus status;
            ZMQMessage* message;
            LHCVersionFlags vfs;
    };
}

#endif
