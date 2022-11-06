#include <lhcluster/notifypublisherparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyPublisherParametersBuilder()
    {
        using namespace boost::python;

        NotifyPublisherParameters
        (NotifyPublisherParametersBuilder::*NotifyPublisherParametersBuilderBuild1)() const =
            &NotifyPublisherParametersBuilder::Build;

        class_< NotifyPublisherParametersBuilder >(
            "NotifyPublisherParametersBuilder",
            init<>() )
            .def( "OK",
                  &NotifyPublisherParametersBuilder::OK )
            .def( "Build",
                  NotifyPublisherParametersBuilderBuild1 )
            .def( "SetInstance",
                  &NotifyPublisherParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &NotifyPublisherParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &NotifyPublisherParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &NotifyPublisherParametersBuilder::GetProcess )
            .def( "SetThread",
                  &NotifyPublisherParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &NotifyPublisherParametersBuilder::GetThread )
            .def( "SetProxyEndpoint",
                  &NotifyPublisherParametersBuilder::SetProxyEndpoint,
                  return_internal_reference<>() )
            .def( "GetProxyEndpoint",
                  &NotifyPublisherParametersBuilder::GetProxyEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfEndpointType",
                  &NotifyPublisherParametersBuilder::SetSelfEndpointType,
                  return_internal_reference<>() )
            .def( "GetSelfEndpointType",
                  &NotifyPublisherParametersBuilder::GetSelfEndpointType )
            .def( "SetSelfEndpointPort",
                  &NotifyPublisherParametersBuilder::SetSelfEndpointPort,
                  return_internal_reference<>() )
            .def( "GetSelfEndpointPort",
                  &NotifyPublisherParametersBuilder::GetSelfEndpointPort )
            .def( "SetSelfEndpointBaseAddress",
                  &NotifyPublisherParametersBuilder::SetSelfEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfEndpointBaseAddress",
                  &NotifyPublisherParametersBuilder::GetSelfEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() );
    }
}
}
