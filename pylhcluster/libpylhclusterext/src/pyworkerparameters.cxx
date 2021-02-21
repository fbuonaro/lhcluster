#include <lhcluster/workerparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonWorkerParameters()
    {
        using namespace boost::python;

        class_< WorkerParameters >(
            "WorkerParameters",
            init<>() )
            .def( init<
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                const Endpoint&,
                Delay,
                Delay,
                Delay >() )
            .def( "__eq__",
                  &WorkerParameters::operator== )
            .def( "SetSelfMessagingEndpoint",
                  &WorkerParameters::SetSelfMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfMessagingEndpoint",
                  &WorkerParameters::GetSelfMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetBrokerMessagingEndpoint",
                  &WorkerParameters::SetBrokerMessagingEndpoint,
                  return_internal_reference<>() )
            .def( "GetBrokerMessagingEndpoint",
                  &WorkerParameters::GetBrokerMessagingEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfControllerEndpoint",
                  &WorkerParameters::SetSelfControllerEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfControllerEndpoint",
                  &WorkerParameters::GetSelfControllerEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfInternalEndpoint",
                  &WorkerParameters::SetSelfInternalEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfInternalEndpoint",
                  &WorkerParameters::GetSelfInternalEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetRequestHandlerInternalEndpoint",
                  &WorkerParameters::SetRequestHandlerInternalEndpoint,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerInternalEndpoint",
                  &WorkerParameters::GetRequestHandlerInternalEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetRequestHandlerControllerEndpoint",
                  &WorkerParameters::SetRequestHandlerControllerEndpoint,
                  return_internal_reference<>() )
            .def( "GetRequestHandlerControllerEndpoint",
                  &WorkerParameters::GetRequestHandlerControllerEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfHeartbeatDelay",
                  &WorkerParameters::SetSelfHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetSelfHeartbeatDelay",
                  &WorkerParameters::GetSelfHeartbeatDelay )
            .def( "SetInitialBrokerHeartbeatDelay",
                  &WorkerParameters::SetInitialBrokerHeartbeatDelay,
                  return_internal_reference<>() )
            .def( "GetInitialBrokerHeartbeatDelay",
                  &WorkerParameters::GetInitialBrokerHeartbeatDelay )
            .def( "SetBrokerPulseDelay",
                  &WorkerParameters::SetBrokerPulseDelay,
                  return_internal_reference<>() )
            .def( "GetBrokerPulseDelay",
                  &WorkerParameters::GetBrokerPulseDelay );
    }
}
}
