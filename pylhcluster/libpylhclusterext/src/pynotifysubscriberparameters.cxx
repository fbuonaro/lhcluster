#include <lhcluster/notifysubscriberparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifySubscriberParameters()
    {
        using namespace boost::python;

        class_< NotifySubscriberParameters >(
            "NotifySubscriberParameters",
            init<>() )
            .def( init< 
                    const Endpoint&,
                    const Endpoint& >() )
            .def( "__eq__",
                  &NotifySubscriberParameters::operator== )
            .def( "SetSelfControllerEndpoint",
                  &NotifySubscriberParameters::SetSelfControllerEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfControllerEndpoint",
                  &NotifySubscriberParameters::GetSelfControllerEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetPublisherEndpoint",
                  &NotifySubscriberParameters::SetPublisherEndpoint,
                  return_internal_reference<>() )
            .def( "GetPublisherEndpoint",
                  &NotifySubscriberParameters::GetPublisherEndpoint,
                  return_value_policy< copy_const_reference >() );
    }
}
}
