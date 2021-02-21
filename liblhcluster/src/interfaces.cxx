#include <lhcluster/ibroker.h>
#include <lhcluster/iclient.h>
#include <lhcluster/iclientproxy.h>
#include <lhcluster/inotifybroker.h>
#include <lhcluster/inotifyhandler.h>
#include <lhcluster/inotifypublisher.h>
#include <lhcluster/inotifysubscriber.h>
#include <lhcluster/irequesthandler.h>
#include <lhcluster/iworker.h>
#include <lhcluster_impl/izmqsocketfactory.h>
#include <lhcluster_impl/zmqsocketsenderreceiver.h>

namespace LHClusterNS
{
    IBroker::~IBroker() {}
    IClient::~IClient() {}
    IClientProxy::~IClientProxy() {}
    INotifyBroker::~INotifyBroker() {}
    INotifyHandler::~INotifyHandler() {}
    INotifyPublisher::~INotifyPublisher() {}
    IRequestHandler::~IRequestHandler() {}
    INotifySubscriber::~INotifySubscriber() {}
    IWorker::~IWorker() {}
    Impl::IZMQSocketFactory::~IZMQSocketFactory() {}
    Impl::IZMQSocketSenderReceiver::~IZMQSocketSenderReceiver() {}
}
