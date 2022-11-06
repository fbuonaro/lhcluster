#include <pylhclusterext/pygilguard.h>
#include <pylhclusterext/pyzmqaddin.h>

#include <lhcluster/client.h>
#include <lhcluster/clientfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace LHClusterNS
{
    namespace Python
    {
        namespace
        {
            boost::shared_ptr< Client > proxyCreateViaFactory(
                ClientFactory& brokerFactory,
                const ClientParameters& p )
            {
                return boost::shared_ptr< Client >(
                    new Client( brokerFactory.CreateConcrete( p ) ),
                    DeleteWithNoGIL< Client > );
            }

            int sendRequestWithMessageBody(
                boost::shared_ptr< Client >& client,
                RequestType requestType,
                RequestId requestId,
                MessageFlags messageFlags,
                PyZMQMessage& pyZMQMessage )
            {
                ZMQMessage* zmqMessage =
                    pyZMQMessage.Owning()
                    ? zmsg_dup( pyZMQMessage.GetUnderlyingMessage() )
                    : pyZMQMessage.GetUnderlyingMessage();
                int ret = client->SendRequest( requestType, requestId, messageFlags, &zmqMessage );
                if ( ret && zmqMessage && pyZMQMessage.Owning() )
                    zmsg_destroy( &zmqMessage );
                return ret;
            }

            int sendAsynchronousRequestWithMessageBody(
                boost::shared_ptr< Client >& client,
                RequestType requestType,
                RequestId requestId,
                PyZMQMessage& pyZMQMessage )
            {
                return sendRequestWithMessageBody(
                    client,
                    requestType,
                    requestId,
                    MessageFlag::Asynchronous,
                    pyZMQMessage );
            }

            int sendSynchronousRequestWithMessageBody(
                boost::shared_ptr< Client >& client,
                RequestType requestType,
                RequestId requestId,
                PyZMQMessage& pyZMQMessage )
            {
                return sendRequestWithMessageBody(
                    client,
                    requestType,
                    requestId,
                    MessageFlag::None,
                    pyZMQMessage );
            }

        }

        void ExportPythonClient()
        {
            using namespace boost::python;

            int
            ( Client:: * RetrieveResponsesNonBlocking )(
                std::vector< RequestResponse >&
                requestResponses,
                int responseLimit ) = &Client::RetrieveResponses;

            int
            ( Client:: * RetrieveResponsesTimeoutS )(
                std::vector< RequestResponse >&requestResponses,
                int responseLimit,
                std::chrono::seconds timeout_s ) = &Client::RetrieveResponses;

            int
            ( Client:: * RetrieveResponsesTimeoutMS )(
                std::vector< RequestResponse >&requestResponses,
                int responseLimit,
                std::chrono::milliseconds timeout_ms ) = &Client::RetrieveResponses;

            int
            ( Client:: * SendAsynchronousRequestEmpty )(
                RequestType requestType,
                RequestId requestId ) = &Client::SendAsynchronousRequest;

            int
            ( Client:: * SendSynchronousRequestEmpty )(
                RequestType requestType,
                RequestId requestId ) = &Client::SendSynchronousRequest;

            int
            ( Client:: * SendRequestEmpty )(
                RequestType requestType,
                RequestId requestId,
                MessageFlags messageFlags ) = &Client::SendRequest;

            class_< std::vector< RequestResponse > >( "RequestResponseList" )
                .def( vector_indexing_suite< std::vector< RequestResponse > >() );

            class_<
                Client,
                boost::noncopyable, boost::shared_ptr< Client > >(
                    "Client", no_init )
                .def( "__init__", make_constructor( &proxyCreateViaFactory ) )
                .def( "Activate", &Client::Activate )
                .def( "Deactivate", &Client::Deactivate )
                .def( "RetrieveResponsesNonBlocking", with< noGIL >( RetrieveResponsesNonBlocking ) )
                .def( "RetrieveResponsesTimeoutS", with< noGIL >( RetrieveResponsesTimeoutS ) )
                .def( "RetrieveResponsesTimeoutMS", with< noGIL >( RetrieveResponsesTimeoutMS ) )
                .def( "SendAsynchronousRequestEmpty", with< noGIL >( SendAsynchronousRequestEmpty ) )
                .def( "SendSynchronousRequestEmpty", with< noGIL >( SendSynchronousRequestEmpty ) )
                .def( "SendRequestEmpty", with< noGIL >( SendRequestEmpty ) )
                .def( "SendAsynchronousRequest", with< noGIL >( sendAsynchronousRequestWithMessageBody ) )
                .def( "SendSynchronousRequest", with< noGIL >( sendSynchronousRequestWithMessageBody ) )
                .def( "SendRequest", with< noGIL >( sendRequestWithMessageBody ) );
        }
    }
}
