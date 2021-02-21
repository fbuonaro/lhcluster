#include <pylhcluster/pygilguard.h>

#include <lhcluster/clientproxy.h>
#include <lhcluster/clientproxyfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    namespace
    {
        boost::shared_ptr< ClientProxy > proxyCreateViaFactory(
            ClientProxyFactory& clientProxyFactory,
            const ClientProxyParameters& p )
        {
            return boost::shared_ptr< ClientProxy >(
                new ClientProxy( clientProxyFactory.CreateConcrete( p ) ),
                DeleteWithNoGIL< ClientProxy > );
        }
    }

    void ExportPythonClientProxy()
    {
        using namespace boost::python;

        class_<
            ClientProxy,
            boost::noncopyable, boost::shared_ptr< ClientProxy > >(
            "ClientProxy", no_init )
            .def( "__init__",       make_constructor( &proxyCreateViaFactory ) )
            .def( "Start",          with< noGIL >( &ClientProxy::Start ) )
            .def( "StartAsPrimary", with< noGIL >( &ClientProxy::StartAsPrimary ) )
            .def( "Stop",           with< noGIL >( &ClientProxy::Stop ) );
    }
}
}
