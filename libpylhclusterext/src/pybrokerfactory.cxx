#include <lhcluster/brokerfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonBrokerFactory()
    {
        using namespace boost::python;

        class_< BrokerFactory, boost::noncopyable >(
            "BrokerFactory",
            init<>() )
            .def( "Create", ( &BrokerFactory::CreateConcrete ) );
    }
}
}
