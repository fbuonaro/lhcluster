#include <lhcluster/notifybrokerparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyBrokerParametersBuilder()
    {
        using namespace boost::python;

        NotifyBrokerParameters
        (NotifyBrokerParametersBuilder::*NotifyBrokerParametersBuilderBuild1)() const =
            &NotifyBrokerParametersBuilder::Build;

        class_< NotifyBrokerParametersBuilder >(
            "NotifyBrokerParametersBuilder",
            init<>() )
            .def( "OK",
                  &NotifyBrokerParametersBuilder::OK )
            .def( "Build",
                  NotifyBrokerParametersBuilderBuild1 )
            .def( "SetInstance",
                  &NotifyBrokerParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &NotifyBrokerParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &NotifyBrokerParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &NotifyBrokerParametersBuilder::GetProcess )
            .def( "SetThread",
                  &NotifyBrokerParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &NotifyBrokerParametersBuilder::GetThread )
            .def( "SetFrontendEndpointType",
                  &NotifyBrokerParametersBuilder::SetFrontendEndpointType,
                  return_internal_reference<>() )
            .def( "GetFrontendEndpointType",
                  &NotifyBrokerParametersBuilder::GetFrontendEndpointType )
            .def( "SetFrontendEndpointBaseAddress",
                  &NotifyBrokerParametersBuilder::SetFrontendEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetFrontendEndpointBaseAddress",
                  &NotifyBrokerParametersBuilder::GetFrontendEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetFrontendEndpointPort",
                  &NotifyBrokerParametersBuilder::SetFrontendEndpointPort,
                  return_internal_reference<>() )
            .def( "GetFrontendEndpointPort",
                  &NotifyBrokerParametersBuilder::GetFrontendEndpointPort )
            .def( "SetBackendEndpointType",
                  &NotifyBrokerParametersBuilder::SetBackendEndpointType,
                  return_internal_reference<>() )
            .def( "GetBackendEndpointType",
                  &NotifyBrokerParametersBuilder::GetBackendEndpointType )
            .def( "SetBackendEndpointBaseAddress",
                  &NotifyBrokerParametersBuilder::SetBackendEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetBackendEndpointBaseAddress",
                  &NotifyBrokerParametersBuilder::GetBackendEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetBackendEndpointPort",
                  &NotifyBrokerParametersBuilder::SetBackendEndpointPort,
                  return_internal_reference<>() )
            .def( "GetBackendEndpointPort",
                  &NotifyBrokerParametersBuilder::GetBackendEndpointPort );
    }
}
}
