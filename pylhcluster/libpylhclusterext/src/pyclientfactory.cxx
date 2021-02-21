#include <lhcluster/clientfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientFactory()
    {
        using namespace boost::python;

        class_< ClientFactory, boost::noncopyable >(
            "ClientFactory",
            init<>() )
            .def( "Create", ( &ClientFactory::CreateConcrete ) );
    }
}
}
