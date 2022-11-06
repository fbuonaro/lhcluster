#include <lhcluster/clientproxyfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonClientProxyFactory()
    {
        using namespace boost::python;

        class_< ClientProxyFactory, boost::noncopyable >(
            "ClientProxyFactory",
            init<>() )
            .def( "Create", ( &ClientProxyFactory::CreateConcrete ) );
    }
}
}
