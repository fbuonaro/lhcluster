#include <lhcluster/brokerparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
      namespace Python
      {
            void ExportPythonBrokerParametersBuilder()
            {
                  using namespace boost::python;

                  BrokerParameters
                  ( BrokerParametersBuilder:: * BrokerParametersBuilderBuild1 )( ) const =
                        &BrokerParametersBuilder::Build;

                  class_< BrokerParametersBuilder >(
                        "BrokerParametersBuilder",
                        init<>() )
                        .def( "OK",
                              &BrokerParametersBuilder::OK )
                        .def( "Build",
                              BrokerParametersBuilderBuild1 )
                        .def( "SetInstance",
                              &BrokerParametersBuilder::SetInstance,
                              return_internal_reference<>() )
                        .def( "GetInstance",
                              &BrokerParametersBuilder::GetInstance )
                        .def( "SetProcess",
                              &BrokerParametersBuilder::SetProcess,
                              return_internal_reference<>() )
                        .def( "GetProcess",
                              &BrokerParametersBuilder::GetProcess )
                        .def( "SetThread",
                              &BrokerParametersBuilder::SetThread,
                              return_internal_reference<>() )
                        .def( "GetThread",
                              &BrokerParametersBuilder::GetThread )
                        .def( "SetFrontendEndpointType",
                              &BrokerParametersBuilder::SetFrontendEndpointType,
                              return_internal_reference<>() )
                        .def( "GetFrontendEndpointType",
                              &BrokerParametersBuilder::GetFrontendEndpointType )
                        .def( "SetFrontendEndpointBaseAddress",
                              &BrokerParametersBuilder::SetFrontendEndpointBaseAddress,
                              return_internal_reference<>() )
                        .def( "GetFrontendEndpointBaseAddress",
                              &BrokerParametersBuilder::GetFrontendEndpointBaseAddress,
                              return_value_policy< copy_const_reference >() )
                        .def( "SetFrontendEndpointPort",
                              &BrokerParametersBuilder::SetFrontendEndpointPort,
                              return_internal_reference<>() )
                        .def( "GetFrontendEndpointPort",
                              &BrokerParametersBuilder::GetFrontendEndpointPort )
                        .def( "SetBackendEndpointType",
                              &BrokerParametersBuilder::SetBackendEndpointType,
                              return_internal_reference<>() )
                        .def( "GetBackendEndpointType",
                              &BrokerParametersBuilder::GetBackendEndpointType )
                        .def( "SetBackendEndpointBaseAddress",
                              &BrokerParametersBuilder::SetBackendEndpointBaseAddress,
                              return_internal_reference<>() )
                        .def( "GetBackendEndpointBaseAddress",
                              &BrokerParametersBuilder::GetBackendEndpointBaseAddress,
                              return_value_policy< copy_const_reference >() )
                        .def( "SetBackendEndpointPort",
                              &BrokerParametersBuilder::SetBackendEndpointPort,
                              return_internal_reference<>() )
                        .def( "GetBackendEndpointPort",
                              &BrokerParametersBuilder::GetBackendEndpointPort )
                        .def( "SetControlEndpointBaseAddress",
                              &BrokerParametersBuilder::SetControlEndpointBaseAddress,
                              return_internal_reference<>() )
                        .def( "GetControlEndpointBaseAddress",
                              &BrokerParametersBuilder::GetControlEndpointBaseAddress,
                              return_value_policy< copy_const_reference >() )
                        .def( "SetSelfHeartbeatDelay",
                              &BrokerParametersBuilder::SetSelfHeartbeatDelay,
                              return_internal_reference<>() )
                        .def( "GetSelfHeartbeatDelay",
                              &BrokerParametersBuilder::GetSelfHeartbeatDelay )
                        .def( "SetMinimumWorkerHeartbeatDelay",
                              &BrokerParametersBuilder::SetMinimumWorkerHeartbeatDelay,
                              return_internal_reference<>() )
                        .def( "GetMinimumWorkerHeartbeatDelay",
                              &BrokerParametersBuilder::GetMinimumWorkerHeartbeatDelay )
                        .def( "SetWorkerPulseDelay",
                              &BrokerParametersBuilder::SetWorkerPulseDelay,
                              return_internal_reference<>() )
                        .def( "GetWorkerPulseDelay",
                              &BrokerParametersBuilder::GetWorkerPulseDelay );
            }
      }
}
