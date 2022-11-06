#include <lhcluster/clientproxyparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientProxyParametersBuilder()
    {
        using namespace boost::python;

        ClientProxyParameters
        (ClientProxyParametersBuilder::*ClientProxyParametersBuilderBuild1)() const =
            &ClientProxyParametersBuilder::Build;

        class_< ClientProxyParametersBuilder >(
            "ClientProxyParametersBuilder",
            init<>() )
            .def( "OK",
                  &ClientProxyParametersBuilder::OK )
            .def( "Build",
                  ClientProxyParametersBuilderBuild1 )
            .def( "SetInstance",
                  &ClientProxyParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &ClientProxyParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &ClientProxyParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &ClientProxyParametersBuilder::GetProcess )
            .def( "SetThread",
                  &ClientProxyParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &ClientProxyParametersBuilder::GetThread )
            .def( "SetSelfBrokerMessagingEndpointType",
                  &ClientProxyParametersBuilder::SetSelfBrokerMessagingEndpointType,
                  return_internal_reference<>() )
            .def( "GetSelfBrokerMessagingEndpointType",
                  &ClientProxyParametersBuilder::GetSelfBrokerMessagingEndpointType )
            .def( "SetSelfBrokerMessagingEndpointBaseAddress",
                  &ClientProxyParametersBuilder::SetSelfBrokerMessagingEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfBrokerMessagingEndpointBaseAddress",
                  &ClientProxyParametersBuilder::GetSelfBrokerMessagingEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfBrokerMessagingEndpointPort",
                  &ClientProxyParametersBuilder::SetSelfBrokerMessagingEndpointPort,
                  return_internal_reference<>() )
            .def( "GetSelfBrokerMessagingEndpointPort",
                  &ClientProxyParametersBuilder::GetSelfBrokerMessagingEndpointPort )
            .def( "SetBrokerEndpoint",
                  &ClientProxyParametersBuilder::SetBrokerEndpoint,
                  return_internal_reference<>() )
            .def( "GetBrokerEndpoint",
                  &ClientProxyParametersBuilder::GetBrokerEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfClientMessagingEndpointType",
                  &ClientProxyParametersBuilder::SetSelfClientMessagingEndpointType,
                  return_internal_reference<>() )
            .def( "GetSelfClientMessagingEndpointType",
                  &ClientProxyParametersBuilder::GetSelfClientMessagingEndpointType )
            .def( "SetSelfClientMessagingEndpointBaseAddress",
                  &ClientProxyParametersBuilder::SetSelfClientMessagingEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfClientMessagingEndpointBaseAddress",
                  &ClientProxyParametersBuilder::GetSelfClientMessagingEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfClientMessagingEndpointPort",
                  &ClientProxyParametersBuilder::SetSelfClientMessagingEndpointPort,
                  return_internal_reference<>() )
            .def( "GetSelfClientMessagingEndpointPort",
                  &ClientProxyParametersBuilder::GetSelfClientMessagingEndpointPort )
            .def( "SetControlEndpointBaseAddress",
                  &ClientProxyParametersBuilder::SetControlEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetControlEndpointBaseAddress",
                  &ClientProxyParametersBuilder::GetControlEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfHeartbeatDelay",
                  &ClientProxyParametersBuilder::SetSelfHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetSelfHeartbeatDelay",
                  &ClientProxyParametersBuilder::GetSelfHeartbeatDelay )
            .def( "SetInitialBrokerHeartbeatDelay",
                  &ClientProxyParametersBuilder::SetInitialBrokerHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetInitialBrokerHeartbeatDelay",
                  &ClientProxyParametersBuilder::GetInitialBrokerHeartbeatDelay )
            .def( "SetBrokerPulseDelay",
                  &ClientProxyParametersBuilder::SetBrokerPulseDelay,
                  return_internal_reference<>() )
            .def( "GetBrokerPulseDelay",
                  &ClientProxyParametersBuilder::GetBrokerPulseDelay );
    }
}
}
