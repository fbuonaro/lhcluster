#include <pylhclusterext/pyexport.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
    namespace Python
    {
        void ExportAll()
        {
            LHClusterNS::Python::ExportPythonBroker();
            LHClusterNS::Python::ExportPythonBrokerFactory();
            LHClusterNS::Python::ExportPythonBrokerParameters();
            LHClusterNS::Python::ExportPythonBrokerParametersBuilder();

            LHClusterNS::Python::ExportPythonClient();
            LHClusterNS::Python::ExportPythonClientFactory();
            LHClusterNS::Python::ExportPythonClientParameters();
            LHClusterNS::Python::ExportPythonClientParametersBuilder();

            LHClusterNS::Python::ExportPythonClientProxy();
            LHClusterNS::Python::ExportPythonClientProxyFactory();
            LHClusterNS::Python::ExportPythonClientProxyParameters();
            LHClusterNS::Python::ExportPythonClientProxyParametersBuilder();

            LHClusterNS::Python::ExportPythonCluster();

            LHClusterNS::Python::ExportPythonLogging();

            LHClusterNS::Python::ExportPythonEndpoint();
            LHClusterNS::Python::ExportPythonEndpointBuilder();

            LHClusterNS::Python::ExportPythonNotifyBroker();
            LHClusterNS::Python::ExportPythonNotifyBrokerFactory();
            LHClusterNS::Python::ExportPythonNotifyBrokerParameters();
            LHClusterNS::Python::ExportPythonNotifyBrokerParametersBuilder();

            LHClusterNS::Python::ExportPythonNotifyPublisher();
            LHClusterNS::Python::ExportPythonNotifyPublisherFactory();
            LHClusterNS::Python::ExportPythonNotifyPublisherParameters();
            LHClusterNS::Python::ExportPythonNotifyPublisherParametersBuilder();

            LHClusterNS::Python::ExportPythonNotifySubscriber();
            LHClusterNS::Python::ExportPythonNotifySubscriberFactory();
            LHClusterNS::Python::ExportPythonNotifySubscriberParameters();
            LHClusterNS::Python::ExportPythonNotifySubscriberParametersBuilder();

            LHClusterNS::Python::ExportPythonRequestResponse();

            LHClusterNS::Python::ExportPythonWorker();
            LHClusterNS::Python::ExportPythonWorkerFactory();
            LHClusterNS::Python::ExportPythonWorkerParameters();
            LHClusterNS::Python::ExportPythonWorkerParametersBuilder();

            LHClusterNS::Python::ExportPythonZMQAddin();

        }
    }
}

namespace boost
{
    namespace python
    {
        BOOST_PYTHON_MODULE( libpylhclusterext )
        {
            LHClusterNS::Python::ExportAll();
            PyEval_InitThreads();
        }
    }
}