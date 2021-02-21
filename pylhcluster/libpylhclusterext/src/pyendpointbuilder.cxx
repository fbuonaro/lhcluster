#include <lhcluster/endpointbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonEndpointBuilder()
    {
        using namespace boost::python;

        Endpoint (EndpointBuilder::*EndpointBuilderBuild1)() const =
            &EndpointBuilder::Build;

        Endpoint (EndpointBuilder::*EndpointBuilderBuild2)(
            int instance, int process, int thread ) const =
            &EndpointBuilder::Build;

        class_< EndpointBuilder >(
            "EndpointBuilder",
            init<>() )
            .def( "OK",
                  &EndpointBuilder::OK )
            .def( "Build",
                  EndpointBuilderBuild1 )
            .def( "Build",
                  EndpointBuilderBuild2 )
            .def( "SetInstance",
                  &EndpointBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &EndpointBuilder::GetInstance )
            .def( "SetProcess",
                  &EndpointBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &EndpointBuilder::GetProcess )
            .def( "SetThread",
                  &EndpointBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &EndpointBuilder::GetThread )
            .def( "SetBaseAddress",
                  &EndpointBuilder::SetBaseAddress,
                  return_internal_reference<>() )
            .def( "GetBaseAddress",
                  &EndpointBuilder::GetBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetPort",
                  &EndpointBuilder::SetPort,
                  return_internal_reference<>() )
            .def( "GetPort",
                  &EndpointBuilder::GetPort )
            .def( "SetEndpointType",
                  &EndpointBuilder::SetEndpointType,
                  return_internal_reference<>() )
            .def( "GetEndpointType",
                  &EndpointBuilder::GetEndpointType )
            .def( "SetEndpoint",
                  &EndpointBuilder::SetEndpoint,
                  return_internal_reference<>() )
            .def( "GetEndpoint",
                  &EndpointBuilder::GetEndpoint,
                  return_value_policy< copy_const_reference >() );
    }
}
}
