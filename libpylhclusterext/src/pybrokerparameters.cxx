#include <lhcluster/brokerparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonBrokerParameters()
    {
        using namespace boost::python;

        class_< BrokerParameters >(
            "BrokerParameters",
            init<>() )
            .def( init< 
                    const Endpoint&,
                    const Endpoint&,
                    const Endpoint&,
                    Delay,
                    Delay,
                    Delay >() )
            .def( "__eq__",
                  &BrokerParameters::operator== )
            .def( "SetFrontendMessagingEndpoint",
                  &BrokerParameters::SetFrontendMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetFrontendMessagingEndpoint",
                  &BrokerParameters::GetFrontendMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetBackendMessagingEndpoint",
                  &BrokerParameters::SetBackendMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetBackendMessagingEndpoint",
                  &BrokerParameters::GetBackendMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetControlEndpoint",
                  &BrokerParameters::SetControlEndpoint,
                  return_internal_reference<>() )
            .def( "GetControlEndpoint",
                  &BrokerParameters::GetControlEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfHeartbeatDelay",
                  &BrokerParameters::SetSelfHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetSelfHeartbeatDelay",
                  &BrokerParameters::GetSelfHeartbeatDelay )
            .def( "SetMinimumWorkerHeartbeatDelay",
                  &BrokerParameters::SetMinimumWorkerHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetMinimumWorkerHeartbeatDelay",
                  &BrokerParameters::GetMinimumWorkerHeartbeatDelay,
                  return_value_policy< return_by_value >() )
            .def( "SetWorkerPulseDelay",
                  &BrokerParameters::SetWorkerPulseDelay,
                  return_internal_reference<>() )
            .def( "GetWorkerPulseDelay",
                  &BrokerParameters::GetWorkerPulseDelay,
                  return_value_policy< return_by_value >() );
    }
}
}
