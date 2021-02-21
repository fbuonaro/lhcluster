#include <lhcluster/notifypublisherfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifyPublisherFactory()
    {
        using namespace boost::python;

        class_< NotifyPublisherFactory, boost::noncopyable >(
            "NotifyPublisherFactory",
            init<>() )
            .def( "Create", ( &NotifyPublisherFactory::CreateConcrete ) );
    }
}
}
