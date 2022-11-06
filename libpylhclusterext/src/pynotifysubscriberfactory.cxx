#include <lhcluster/notifysubscriberfactory.h>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonNotifySubscriberFactory()
    {
        using namespace boost::python;

        // TODO - will not work unless we update boost to support unique_ptr
        //        and then add something for INotifySubscriber
        // NotifySubscriber
        // (NotifySubscriberFactory::*Create1)(
        //     const NotifySubscriberParameters& p,
        //     std::unique_ptr< INotifyHandler > nh ) =
        //     &NotifySubscriberFactory::CreateConcrete;

        // NotifySubscriber
        // (NotifySubscriberFactory::*Create2)(
        //     const NotifySubscriberParameters& p,
        //     const std::vector< NotificationType >& _subscriptions,
        //     std::unique_ptr< INotifyHandler > nh ) =
        //     &NotifySubscriberFactory::CreateConcrete;


        class_< NotifySubscriberFactory, boost::noncopyable >(
            "NotifySubscriberFactory",
            init<>() );
            // .def( "Create1", ( Create1 ) )
            // .def( "Create2", ( Create2 ) );
    }
}
}
