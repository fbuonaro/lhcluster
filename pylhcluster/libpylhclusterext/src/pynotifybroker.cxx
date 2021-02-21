#include <pylhcluster/pygilguard.h>

#include <lhcluster/notifybroker.h>
#include <lhcluster/notifybrokerfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    namespace
    {
        boost::shared_ptr< NotifyBroker > proxyCreateViaFactory(
            NotifyBrokerFactory& notifyBrokerFactory,
            const NotifyBrokerParameters& p )
        {
            return boost::shared_ptr< NotifyBroker >(
                new NotifyBroker( notifyBrokerFactory.CreateConcrete( p ) ),
                DeleteWithNoGIL< NotifyBroker > );
        }
                                                
    }

    void ExportPythonNotifyBroker()
    {
        using namespace boost::python;

        class_<
            NotifyBroker,
            boost::noncopyable, boost::shared_ptr< NotifyBroker > >(
            "NotifyBroker", no_init )
            .def( "__init__",       make_constructor( &proxyCreateViaFactory ) )
            .def( "Start",          with< noGIL >( &NotifyBroker::Start ) )
            .def( "Stop",           with< noGIL >( &NotifyBroker::Stop ) );
    }
}
}
