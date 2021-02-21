#!/usr/bin/python3.6

from pylhcluster import plhc_ext
from pylhcluster.plhc_api.config.actor_config import ClusterActorFactory
from pylhcluster.plhc_api.quickstart import CreateNotifyBrokerParametersBuilder

class NotifyBrokerActorFactory( ClusterActorFactory ):
    NOTIFYBROKER_PARAM_FIELDS = \
    [
        'frontendEndpointType',
        'frontendEndpointBaseAddress',
        'frontendEndpointPort',
        'backendEndpointType',
        'backendEndpointBaseAddress',
        'backendEndpointPort'
    ]

    TYPE_NAME = "notifybroker"

    def __init__( self, notifyBrokerConfig ):
        """
        <NotifyBroker>:
        {
            ?cluster: <Cluster>
        }
        """
        super().__init__( notifyBrokerConfig.get( 'cluster', {} ) )

        nbFactory = plhc_ext.NotifyBrokerFactory()
        self.genActor = lambda param : plhc_ext.NotifyBroker( nbFactory, param )

        self.type = NotifyBrokerActorFactory.TYPE_NAME

        self.parameterBuilder = CreateNotifyBrokerParametersBuilder(
            **{ f : v for f,v in notifyBrokerConfig.items() \
                if f in NotifyBrokerActorFactory.NOTIFYBROKER_PARAM_FIELDS } )
