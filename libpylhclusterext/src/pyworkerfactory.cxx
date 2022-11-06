#include <lhcluster/workerfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonWorkerFactory()
    {
        using namespace boost::python;

        // TODO - see notes in pynotifysubscriberfactory.cxx

        class_< WorkerFactory, boost::noncopyable >(
            "WorkerFactory",
            init<>() );
    }
}
}
