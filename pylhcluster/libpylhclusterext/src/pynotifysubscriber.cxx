#include <pylhcluster/pygilguard.h>
#include <pylhcluster/pyzmqaddin.h>

#include <lhcluster/notifysubscriber.h>
#include <lhcluster/notifysubscriberfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace LHClusterNS
{
namespace Python
{
    namespace
    {
        // Implemenating an interface in Python to be called by C++ has three parts:
        // 1) Create a separate interface which mirrors the 
        // Wrapper around INotifyHandler interface that is eventually exposed to Python
        // and implemented in Python
        class PyINotifyHandler
        {
            public:
                virtual ~PyINotifyHandler()
                {
                }

                virtual RequestStatus PyProcess(
                    const NotificationType& notificationType,
                    const PyZMQMessage& notificationMsg ) = 0;
        };

        // Wrapper around Python Implementation of PyINotifyHandler as a INotifyHandler impl
        // That is handed to C++ with logic to translate between the C++ and the exposed python
        class PyINotifyHandlerWrapper : public PyINotifyHandler, public INotifyHandler
        {
            public:
                PyINotifyHandlerWrapper( boost::python::object _pyINotifyHandlerImpl )
                :   PyINotifyHandler()
                ,   INotifyHandler()
                ,   pyINotifyHandlerImpl( new boost::python::object( _pyINotifyHandlerImpl ) )
                ,   getPyINotifyHandlerImpl( *pyINotifyHandlerImpl )
                {
                    if( !getPyINotifyHandlerImpl.check() )
                    {
                        throw std::runtime_error(
                            "handler not an instance of PyINotifyHandler" );
                    }
                }

                ~PyINotifyHandlerWrapper()
                {
                    GIL getGIL;
                    pyINotifyHandlerImpl.reset();
                }

                RequestStatus Process(
                    const NotificationType& notificationType,
                    ZMQMessage* notificationMsg )
                {
                    GIL getGIL;
                    PyZMQMessage pyNotificationMsg( notificationMsg );
                    return PyProcess( notificationType, pyNotificationMsg );
                }

                RequestStatus PyProcess(
                    const NotificationType& notificationType,
                    const PyZMQMessage& pyNotificationMsg )
                {
                    PyINotifyHandler& handler( getPyINotifyHandlerImpl() );
                    return handler.PyProcess( notificationType, pyNotificationMsg );
                }

            private:
                std::unique_ptr< boost::python::object > pyINotifyHandlerImpl;
                // TODO - not 100% sure if this should be a reference or not
                boost::python::extract< PyINotifyHandler& > getPyINotifyHandlerImpl;
        };

        // The python implementation of PyINotifyHandler
        class PyINotifyHandlerExposed
        :   public PyINotifyHandler, public boost::python::wrapper< PyINotifyHandler >
        {
            public:
                PyINotifyHandlerExposed()
                :   PyINotifyHandler()
                {
                }

                RequestStatus PyProcess(
                    const NotificationType& notificationType,
                    const PyZMQMessage& pyNotificationMsg )
                {
                    return this->get_override( "PyProcess" )(
                        notificationType, pyNotificationMsg );
                }
        };

        boost::shared_ptr< NotifySubscriber > proxyCreateViaFactory(
            NotifySubscriberFactory& notifySubscriberFactory,
            const NotifySubscriberParameters& p,
            const std::vector< NotificationType >& subscriptions,
            boost::python::object pyINotifyHandlerImpl )
        {
            // TODO - update to try/catch, currently will leak memory every time someone
            //        provides a bad INotifyHandler implementation and the ctor throws
            return boost::shared_ptr< NotifySubscriber >(
                new NotifySubscriber(
                    notifySubscriberFactory.CreateConcrete(
                        p, 
                        subscriptions,
                        std::unique_ptr< INotifyHandler >(
                            new PyINotifyHandlerWrapper( pyINotifyHandlerImpl ) ) ) ),
                DeleteWithNoGIL< NotifySubscriber > );
        }
    }

    void ExportPythonNotifySubscriber()
    {
        using namespace boost::python;

        class_< std::vector< NotificationType > >( "NotificationTypeList" )
            .def( vector_indexing_suite< std::vector< NotificationType > >() );

        class_< PyINotifyHandlerExposed, boost::noncopyable >(
            "PyINotifyHandler", init<>() )
            .def( "PyProcess", pure_virtual( &PyINotifyHandlerExposed::PyProcess ) );

        class_<
            NotifySubscriber,
            boost::noncopyable, boost::shared_ptr< NotifySubscriber > >(
            "NotifySubscriber", no_init )
            .def( "__init__",       make_constructor( &proxyCreateViaFactory ) )
            .def( "Start",          with< noGIL >( &NotifySubscriber::Start ) )
            .def( "StartAsPrimary", with< noGIL >( &NotifySubscriber::StartAsPrimary ) )
            .def( "Stop",           with< noGIL >( &NotifySubscriber::Stop ) )
            .def( "Subscribe",      with< noGIL >( &NotifySubscriber::Subscribe ) )
            .def( "UnSubscribe",    with< noGIL >( &NotifySubscriber::UnSubscribe ) );
    }
}
}
