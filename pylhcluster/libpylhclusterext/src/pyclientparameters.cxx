#include <lhcluster/clientparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientParameters()
    {
        using namespace boost::python;

        class_< ClientParameters >(
            "ClientParameters",
            init<>() )
            .def( init< 
                    const Endpoint&,
                    const Endpoint& >() )
            .def( "__eq__",
                  &ClientParameters::operator== )
            .def( "SetClientMessagingEndpoint",
                  &ClientParameters::SetClientMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetClientMessagingEndpoint",
                  &ClientParameters::GetClientMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetClientMessagingEndpoint",
                  &ClientParameters::SetClientMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetClientMessagingEndpoint",
                  &ClientParameters::GetClientMessagingEndpoint,
                  return_value_policy< copy_const_reference >() );

    }
}
}
