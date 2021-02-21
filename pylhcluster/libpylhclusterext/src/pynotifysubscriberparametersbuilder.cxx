#include <lhcluster/notifysubscriberparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifySubscriberParametersBuilder()
    {
        using namespace boost::python;

        NotifySubscriberParameters
        (NotifySubscriberParametersBuilder::*NotifySubscriberParametersBuilderBuild1)() const =
            &NotifySubscriberParametersBuilder::Build;

        class_< NotifySubscriberParametersBuilder >(
            "NotifySubscriberParametersBuilder",
            init<>() )
            .def( "OK",
                  &NotifySubscriberParametersBuilder::OK )
            .def( "Build",
                  NotifySubscriberParametersBuilderBuild1 )
            .def( "SetInstance",
                  &NotifySubscriberParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &NotifySubscriberParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &NotifySubscriberParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &NotifySubscriberParametersBuilder::GetProcess )
            .def( "SetThread",
                  &NotifySubscriberParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &NotifySubscriberParametersBuilder::GetThread )
            .def( "SetSelfControllerEndpointBaseAddress",
                  &NotifySubscriberParametersBuilder::SetSelfControllerEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfControllerEndpointBaseAddress",
                  &NotifySubscriberParametersBuilder::GetSelfControllerEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetPublisherEndpoint",
                  &NotifySubscriberParametersBuilder::SetPublisherEndpoint,
                  return_internal_reference<>() )
            .def( "GetPublisherEndpoint",
                  &NotifySubscriberParametersBuilder::GetPublisherEndpoint,
                  return_value_policy< copy_const_reference >() );
    }
}
}
