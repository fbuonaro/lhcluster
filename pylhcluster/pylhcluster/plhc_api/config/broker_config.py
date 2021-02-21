#!/usr/bin/python3.6

from pylhcluster import plhc_ext
from pylhcluster.plhc_api.config.actor_config import ClusterActorFactory
from pylhcluster.plhc_api.quickstart import CreateBrokerParametersBuilder

class BrokerActorFactory( ClusterActorFactory ):
    BROKER_PARAM_FIELDS = \
    [
        'frontendEndpointType',
        'frontendEndpointBaseAddress',
        'frontendEndpointPort',
        'backendEndpointType',
        'backendEndpointBaseAddress',
        'backendEndpointPort',
        'controlEndpointBaseAddress',
        'selfHeartbeatDelayS',
        'minWorkerHeartbeatDelayS',
        'workerPulseDelayS'
    ]

    TYPE_NAME = "broker"

    def __init__( self, brokerConfig ):
        """
        <Broker>:
        {
            ?cluster: <Cluster>
        }
        """
        super().__init__( brokerConfig.get( 'cluster', {} ) )

        brokerFactory = plhc_ext.BrokerFactory()
        self.genActor = lambda param : plhc_ext.Broker( brokerFactory, param )

        self.type = BrokerActorFactory.TYPE_NAME

        self.parameterBuilder = CreateBrokerParametersBuilder(
            **{ f : v for f,v in brokerConfig.items() \
                if f in BrokerActorFactory.BROKER_PARAM_FIELDS } )
