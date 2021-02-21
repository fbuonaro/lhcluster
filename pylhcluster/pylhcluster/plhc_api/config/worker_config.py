#!/usr/bin/python3.6

from importlib import import_module

from pylhcluster.plhc_api.config.actor_config import ClusterActorFactory
from pylhcluster.plhc_api.quickstart import CreateEndpointFromDict
from pylhcluster.plhc_api.quickstart import CreateWorkerFromParameters
from pylhcluster.plhc_api.quickstart import CreateWorkerParametersBuilder

# TODO - HACK to exposed the PyIRequestHandler implementation
# by providing a GetRequestHandler alongside expected Start/Stop
class WorkerWrapper():
    def __init__( self, workerParams, requestHandlerImpl ):
        self.requestHandlerImpl = requestHandlerImpl
        self.worker = CreateWorkerFromParameters( workerParams, self.requestHandlerImpl )

    def Start( self ):
        self.worker.Start()

    def Stop( self ):
        self.worker.Stop()

    def GetRequestHandler( self ):
        return self.requestHandlerImpl

class WorkerActorFactory( ClusterActorFactory ):
    WORKER_PARAM_FIELDS = \
    [
        'selfMessagingEndpointType',
        'selfMessagingEndpointBaseAddress',
        'selfMessagingEndpointPort',
        'selfControllerEndpointBaseAddress',
        'selfInternalEndpointType',
        'selfInternalEndpointBaseAddress',
        'selfInternalEndpointPort',
        'requestHandlerControllerEndpointBaseAddress',
        'requestHandlerInternalEndpointType',
        'requestHandlerInternalEndpointBaseAddress',
        'requestHandlerInternalEndpointPort',
        'selfHeartbeatDelayS',
        'initialBrokerHeartbeatDelayS',
        'brokerPulseDelayS'
    ]

    TYPE_NAME = "worker"

    def CreateWorkerFromParameters( self, workerParams ):
        requestHandlerImpl = \
            self.implClass(
                workerParams,
                self.clusterConfig,
                implConfig=self.implConfig,
                instanceNum=self.instance,
                processNum=self.process,
                threadNum=self.thread )
        worker = \
            WorkerWrapper(
                workerParams,
                requestHandlerImpl )
        return worker
     

    def __init__( self, clusterConfig, workerConfig ):
        """
        <Worker>:
        {
            ?cluster: <Cluster>
            brokerBackendEndpoint : <string>
            impl : 
            {
                module: <string | path to module containing class>
                class: <string | name of class in module>
            }
            ?implConfig : <dict | custom config for impl class>
        }

        The impl must be an PyIRequestHandler subclass constructable as follows:
            RequestHandlerImpl(
                workerParams,
                clusterConfig,
                implConfig=None,
                instanceNum=None,
                processNum=None,
                threadNum=None )
            
        """
        super().__init__( workerConfig.get( 'cluster', {} ) )

        brokerBackendEndpoint = \
            CreateEndpointFromDict( workerConfig[ 'brokerBackendEndpoint' ] )

        self.parameterBuilder = CreateWorkerParametersBuilder(
            brokerBackendEndpoint,
            **{ f : v for f,v in workerConfig.items() \
                if f in WorkerActorFactory.WORKER_PARAM_FIELDS } )

        self.implClass = \
            getattr(
                import_module( workerConfig[ 'impl' ][ 'module' ] ),
                workerConfig[ 'impl' ][ 'class' ] )

        self.clusterConfig = clusterConfig
        self.implConfig = workerConfig.get( 'implConfig', None )

        self.type = WorkerActorFactory.TYPE_NAME

        self.genActor = self.CreateWorkerFromParameters
