#ifndef __LHCLUSTER_PYTHONMODULE_H__
#define __LHCLUSTER_PYTHONMODULE_H__

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonEndpoint();
    void ExportPythonEndpointBuilder();

    void ExportPythonBroker();
    void ExportPythonBrokerFactory();
    void ExportPythonBrokerParameters();
    void ExportPythonBrokerParametersBuilder();

    void ExportPythonClient();
    void ExportPythonClientFactory();
    void ExportPythonClientParameters();
    void ExportPythonClientParametersBuilder();

    void ExportPythonClientProxy();
    void ExportPythonClientProxyFactory();
    void ExportPythonClientProxyParameters();
    void ExportPythonClientProxyParametersBuilder();

    void ExportPythonCluster();

    void ExportPythonLogging();

    void ExportPythonNotifyBroker();
    void ExportPythonNotifyBrokerFactory();
    void ExportPythonNotifyBrokerParameters();
    void ExportPythonNotifyBrokerParametersBuilder();

    void ExportPythonNotifyPublisher();
    void ExportPythonNotifyPublisherFactory();
    void ExportPythonNotifyPublisherParameters();
    void ExportPythonNotifyPublisherParametersBuilder();

    void ExportPythonNotifySubscriber();
    void ExportPythonNotifySubscriberFactory();
    void ExportPythonNotifySubscriberParameters();
    void ExportPythonNotifySubscriberParametersBuilder();

    void ExportPythonRequestResponse();

    void ExportPythonWorker();
    void ExportPythonWorkerFactory();
    void ExportPythonWorkerParameters();
    void ExportPythonWorkerParametersBuilder();

    void ExportPythonZMQAddin();
}
}

#endif
