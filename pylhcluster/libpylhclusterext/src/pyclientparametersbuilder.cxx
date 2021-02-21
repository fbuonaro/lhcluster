#include <lhcluster/clientparametersbuilder.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientParametersBuilder()
    {
        using namespace boost::python;

        ClientParameters
        (ClientParametersBuilder::*ClientParametersBuilderBuild1)() const =
            &ClientParametersBuilder::Build;

        class_< ClientParametersBuilder >(
            "ClientParametersBuilder",
            init<>() )
            .def( "OK",
                  &ClientParametersBuilder::OK )
            .def( "Build",
                  ClientParametersBuilderBuild1 )
            .def( "SetInstance",
                  &ClientParametersBuilder::SetInstance,
                  return_internal_reference<>() )
            .def( "GetInstance",
                  &ClientParametersBuilder::GetInstance )
            .def( "SetProcess",
                  &ClientParametersBuilder::SetProcess,
                  return_internal_reference<>() )
            .def( "GetProcess",
                  &ClientParametersBuilder::GetProcess )
            .def( "SetThread",
                  &ClientParametersBuilder::SetThread,
                  return_internal_reference<>() )
            .def( "GetThread",
                  &ClientParametersBuilder::GetThread )
            .def( "SetClientEndpointType",
                  &ClientParametersBuilder::SetClientEndpointType,
                  return_internal_reference<>() )
            .def( "GetClientEndpointType",
                  &ClientParametersBuilder::GetClientEndpointType )
            .def( "SetClientEndpointBaseAddress",
                  &ClientParametersBuilder::SetClientEndpointBaseAddress,
                  return_internal_reference<>() )
            .def( "GetClientEndpointBaseAddress",
                  &ClientParametersBuilder::GetClientEndpointBaseAddress,
                  return_value_policy< copy_const_reference >() )
            .def( "SetClientEndpointPort",
                  &ClientParametersBuilder::SetClientEndpointPort,
                  return_internal_reference<>() )
            .def( "GetClientEndpointPort",
                  &ClientParametersBuilder::GetClientEndpointPort )
            .def( "SetClientProxyEndpoint",
                  &ClientParametersBuilder::SetClientProxyEndpoint,
                  return_internal_reference<>() )
            .def( "GetClientProxyEndpoint",
                  &ClientParametersBuilder::GetClientProxyEndpoint,
                  return_value_policy< copy_const_reference >() );


    }
}
}
