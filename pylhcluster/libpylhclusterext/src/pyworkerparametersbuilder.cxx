#include <lhcluster/workerparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonWorkerParametersBuilder()
    {
        using namespace boost::python;

        WorkerParameters
        (WorkerParametersBuilder::*WorkerParametersBuilderBuild1)() const =
            &WorkerParametersBuilder::Build;

        class_< WorkerParametersBuilder >(
            "WorkerParametersBuilder",
            init<>() )
            .def( "OK",
                  &WorkerParametersBuilder::OK )
            .def( "Build",
                  WorkerParametersBuilderBuild1 )
            .def( "SetInstance",
                  &WorkerParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &WorkerParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &WorkerParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &WorkerParametersBuilder::GetProcess )
            .def( "SetThread",
                  &WorkerParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &WorkerParametersBuilder::GetThread )
            .def( "SetSelfMessagingEndpointPort",
                  &WorkerParametersBuilder::SetSelfMessagingEndpointPort,
                  return_internal_reference<>() )
            .def( "GetSelfMessagingEndpointPort",
                  &WorkerParametersBuilder::GetSelfMessagingEndpointPort )
            .def( "SetSelfMessagingEndpointBaseAddress",
                  &WorkerParametersBuilder::SetSelfMessagingEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfMessagingEndpointBaseAddress",
                  &WorkerParametersBuilder::GetSelfMessagingEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfMessagingEndpointType",
                  &WorkerParametersBuilder::SetSelfMessagingEndpointType,
                  return_internal_reference<>() )
            .def( "GetSelfMessagingEndpointType",
                  &WorkerParametersBuilder::GetSelfMessagingEndpointType )
            .def( "SetBrokerMessagingEndpoint",
                  &WorkerParametersBuilder::SetBrokerMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetBrokerMessagingEndpoint",
                  &WorkerParametersBuilder::GetBrokerMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfControllerEndpointBaseAddress",
                  &WorkerParametersBuilder::SetSelfControllerEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfControllerEndpointBaseAddress",
                  &WorkerParametersBuilder::GetSelfControllerEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfInternalEndpointPort",
                  &WorkerParametersBuilder::SetSelfInternalEndpointPort,
                  return_internal_reference<>() )
            .def( "GetSelfInternalEndpointPort",
                  &WorkerParametersBuilder::GetSelfInternalEndpointPort )
            .def( "SetSelfInternalEndpointBaseAddress",
                  &WorkerParametersBuilder::SetSelfInternalEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetSelfInternalEndpointBaseAddress",
                  &WorkerParametersBuilder::GetSelfInternalEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfInternalEndpointType",
                  &WorkerParametersBuilder::SetSelfInternalEndpointType,
                  return_internal_reference<>() )
            .def( "GetSelfInternalEndpointType",
                  &WorkerParametersBuilder::GetSelfInternalEndpointType )
            .def( "SetRequestHandlerControllerEndpointBaseAddress",
                  &WorkerParametersBuilder::SetRequestHandlerControllerEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerControllerEndpointBaseAddress",
                  &WorkerParametersBuilder::GetRequestHandlerControllerEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetRequestHandlerInternalEndpointPort",
                  &WorkerParametersBuilder::SetRequestHandlerInternalEndpointPort,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerInternalEndpointPort",
                  &WorkerParametersBuilder::GetRequestHandlerInternalEndpointPort )
            .def( "SetRequestHandlerInternalEndpointBaseAddress",
                  &WorkerParametersBuilder::SetRequestHandlerInternalEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerInternalEndpointBaseAddress",
                  &WorkerParametersBuilder::GetRequestHandlerInternalEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetRequestHandlerInternalEndpointType",
                  &WorkerParametersBuilder::SetRequestHandlerInternalEndpointType,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerInternalEndpointType",
                  &WorkerParametersBuilder::GetRequestHandlerInternalEndpointType )
            .def( "SetSelfHeartbeatDelay",
                  &WorkerParametersBuilder::SetSelfHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetSelfHeartbeatDelay",
                  &WorkerParametersBuilder::GetSelfHeartbeatDelay )
            .def( "SetInitialBrokerHeartbeatDelay",
                  &WorkerParametersBuilder::SetInitialBrokerHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetInitialBrokerHeartbeatDelay",
                  &WorkerParametersBuilder::GetInitialBrokerHeartbeatDelay )
            .def( "SetBrokerPulseDelay",
                  &WorkerParametersBuilder::SetBrokerPulseDelay,
                  return_internal_reference<>() )
            .def( "GetBrokerPulseDelay",
                  &WorkerParametersBuilder::GetBrokerPulseDelay );
    }
}
}
