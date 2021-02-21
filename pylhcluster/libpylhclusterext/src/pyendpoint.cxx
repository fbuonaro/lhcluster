#include <lhcluster/endpoint.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonEndpoint()
    {
        using namespace boost::python;

        enum_< LHClusterNS::EndpointType >(
            "EndpointType" )
            .value( "None",         LHClusterNS::EndpointType::None )
            .value( "TCP",          LHClusterNS::EndpointType::TCP )
            .value( "InterThread",  LHClusterNS::EndpointType::InterThread )
            .value( "InterProcess", LHClusterNS::EndpointType::InterProcess );

        class_< LHClusterNS::Endpoint >(
            "Endpoint",
            init<>() )
            .def( init< const LHClusterNS::EndpointType&,
                        const char* >() )
            .def( "path",
                  &LHClusterNS::Endpoint::path )
            .def( "type",
                  &LHClusterNS::Endpoint::type )
            .def( "__eq__",
                  &LHClusterNS::Endpoint::operator== );
    }
}
}
