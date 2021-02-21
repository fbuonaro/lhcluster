#include <lhcluster/notifypublisherparameters.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyPublisherParameters()
    {
        using namespace boost::python;

        class_< NotifyPublisherParameters >(
            "NotifyPublisherParameters",
            init<>() )
            .def( init< 
                    const Endpoint&,
                    const Endpoint& >() )
            .def( "__eq__",
                  &NotifyPublisherParameters::operator== )
            .def( "SetProxyEndpoint",
                  &NotifyPublisherParameters::SetProxyEndpoint,
                  return_internal_reference<>() )
            .def( "GetProxyEndpoint",
                  &NotifyPublisherParameters::GetProxyEndpoint,
                  return_value_policy< copy_const_reference >() )
            .def( "SetSelfEndpoint",
                  &NotifyPublisherParameters::SetSelfEndpoint,
                  return_internal_reference<>() )
            .def( "GetSelfEndpoint",
                  &NotifyPublisherParameters::GetSelfEndpoint,
                  return_value_policy< copy_const_reference >() );
    }
}
}
