#include <lhcluster/clientproxyparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientProxyParameters()
    {
        using namespace boost::python;

        class_< ClientProxyParameters >(
            "ClientProxyParameters",
            init<>() )
            .def( init< 
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                Delay,
                Delay,
                Delay >() )
            .def( "__eq__",
                  &ClientProxyParameters::operator== )
            .def( "SetSelfBrokerMessagingEndpoint",
                  &ClientProxyParameters::SetSelfBrokerMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfBrokerMessagingEndpoint",
                  &ClientProxyParameters::GetSelfBrokerMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetBrokerMessagingEndpoint",
                  &ClientProxyParameters::SetBrokerMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetBrokerMessagingEndpoint",
                  &ClientProxyParameters::GetBrokerMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfControllerEndpoint",
                  &ClientProxyParameters::SetSelfControllerEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfControllerEndpoint",
                  &ClientProxyParameters::GetSelfControllerEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfClientMessagingEndpoint",
                  &ClientProxyParameters::SetSelfClientMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfClientMessagingEndpoint",
                  &ClientProxyParameters::GetSelfClientMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfHeartbeatDelay",
                  &ClientProxyParameters::SetSelfHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetSelfHeartbeatDelay",
                  &ClientProxyParameters::GetSelfHeartbeatDelay ) 
            .def( "SetInitialBrokerHeartbeatDelay",
                  &ClientProxyParameters::SetInitialBrokerHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetInitialBrokerHeartbeatDelay",
                  &ClientProxyParameters::GetInitialBrokerHeartbeatDelay ) 
            .def( "SetBrokerPulseDelay",
                  &ClientProxyParameters::SetBrokerPulseDelay,
                  return_internal_reference<>() )
            .def( "GetBrokerPulseDelay",
                  &ClientProxyParameters::GetBrokerPulseDelay );
    }
}
}
