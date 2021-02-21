#include <pylhcluster/pyzmqaddin.h>

#include <lhcluster/requestresponse.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    namespace
    {
        PyZMQMessage getMessageBody( LHClusterNS::RequestResponse& requestResponse )
        {
            return PyZMQMessage( requestResponse.GetMessageBody(), false );
        }
    }

    void ExportPythonRequestResponse()
    {
        using namespace boost::python;

        enum_< LHClusterNS::RequestStatus >(
            "RequestStatus" )
        .value( "None",         LHClusterNS::RequestStatus::None )
        .value( "New",          LHClusterNS::RequestStatus::New )
        .value( "InProgress",   LHClusterNS::RequestStatus::InProgress )
        .value( "Succeeded",    LHClusterNS::RequestStatus::Succeeded )
        .value( "Failed",       LHClusterNS::RequestStatus::Failed )
        .value( "FatalError",   LHClusterNS::RequestStatus::FatalError )
        .value( "Rejected",     LHClusterNS::RequestStatus::Rejected )
        .value( "WorkerDeath",  LHClusterNS::RequestStatus::Rejected )
        .value( "Redirected",   LHClusterNS::RequestStatus::Rejected );

        // GetMessageBody - will keep RequestResponse alive because it is a view into 
        //                  message owned by RequestResponse
        class_< LHClusterNS::RequestResponse >(
            "RequestResponse", no_init )
        .def( "GetRequestId",
              &LHClusterNS::RequestResponse::GetRequestId )
        .def( "GetRequestType",
              &LHClusterNS::RequestResponse::GetRequestType )
        .def( "GetRequestStatus",
              &LHClusterNS::RequestResponse::GetRequestStatus )
        .def( "GetMessageBody",
              getMessageBody,
              with_custodian_and_ward_postcall< 0, 1 >() )
        .def( "GetVersionAndFlags",
              &LHClusterNS::RequestResponse::GetVersionAndFlags,
              return_value_policy< return_by_value >() );
    }
}
}
