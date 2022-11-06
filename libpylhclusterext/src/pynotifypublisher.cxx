#include <pylhclusterext/pygilguard.h>
#include <pylhclusterext/pyzmqaddin.h>

#include <lhcluster/notifypublisher.h>
#include <lhcluster/notifypublisherfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
    namespace Python
    {
        namespace
        {
            boost::shared_ptr< NotifyPublisher > proxyCreateViaFactory(
                NotifyPublisherFactory& notifyPublisherFactory,
                const NotifyPublisherParameters& p )
            {
                return boost::shared_ptr< NotifyPublisher >(
                    new NotifyPublisher( notifyPublisherFactory.CreateConcrete( p ) ),
                    DeleteWithNoGIL< NotifyPublisher > );
            }

            int publishTypeMessage(
                boost::shared_ptr< NotifyPublisher >& publisher,
                const NotificationType& notifyType,
                PyZMQMessage& pyZMQMessage )
            {
                ZMQMessage* zmqMessage =
                    pyZMQMessage.Owning()
                    ? zmsg_dup( pyZMQMessage.GetUnderlyingMessage() )
                    : pyZMQMessage.GetUnderlyingMessage();
                int ret = publisher->Publish( notifyType, &zmqMessage );
                if ( ret && zmqMessage && pyZMQMessage.Owning() )
                    zmsg_destroy( &zmqMessage );
                return ret;
            }

            int publishTypeIdMessage(
                boost::shared_ptr< NotifyPublisher >& publisher,
                const NotificationType& notifyType,
                RequestId requestId,
                PyZMQMessage& pyZMQMessage )
            {
                ZMQMessage* zmqMessage =
                    pyZMQMessage.Owning()
                    ? zmsg_dup( pyZMQMessage.GetUnderlyingMessage() )
                    : pyZMQMessage.GetUnderlyingMessage();
                int ret = publisher->Publish( notifyType, requestId, &zmqMessage );
                if ( ret && zmqMessage && pyZMQMessage.Owning() )
                    zmsg_destroy( &zmqMessage );
                return ret;
            }
        }

        void ExportPythonNotifyPublisher()
        {
            using namespace boost::python;

            int
            ( NotifyPublisher:: * PublishType )(
                const NotificationType & notifyType ) = &NotifyPublisher::Publish;

            int
            ( NotifyPublisher:: * PublishTypeId )(
                const NotificationType & notifyType,
                RequestId requestId ) = &NotifyPublisher::Publish;

            class_<
                NotifyPublisher,
                boost::noncopyable, boost::shared_ptr< NotifyPublisher > >(
                    "NotifyPublisher", no_init )
                .def( "__init__", make_constructor( &proxyCreateViaFactory ) )
                .def( "PublishType", with< noGIL >( PublishType ) )
                .def( "PublishTypeId", with< noGIL >( PublishTypeId ) )
                .def( "PublishTypeMessage", with< noGIL >( &publishTypeMessage ) )
                .def( "PublishTypeIdMessage", with< noGIL >( &publishTypeIdMessage ) );
        }
    }
}
