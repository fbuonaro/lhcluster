#include <lhcluster/notifybrokerfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyBrokerFactory()
    {
        using namespace boost::python;

        class_< NotifyBrokerFactory, boost::noncopyable >(
            "NotifyBrokerFactory",
            init<>() )
            .def( "Create", ( &NotifyBrokerFactory::CreateConcrete ) );
    }
}
}
