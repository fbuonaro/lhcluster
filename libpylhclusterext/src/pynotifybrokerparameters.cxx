#include <lhcluster/notifybrokerparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyBrokerParameters()
    {
        using namespace boost::python;

        class_< NotifyBrokerParameters >(
            "NotifyBrokerParameters",
            init<>() )
            .def( init< 
                    const Endpoint&,
                    const Endpoint& >() )
            .def( "__eq__",
                  &NotifyBrokerParameters::operator== )
            .def( "SetFrontendMessagingEndpoint",
                  &NotifyBrokerParameters::SetFrontendMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetFrontendMessagingEndpoint",
                  &NotifyBrokerParameters::GetFrontendMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetBackendMessagingEndpoint",
                  &NotifyBrokerParameters::SetBackendMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetBackendMessagingEndpoint",
                  &NotifyBrokerParameters::GetBackendMessagingEndpoint,
                  return_value_policy< copy_const_reference >() );
    }
}
}
