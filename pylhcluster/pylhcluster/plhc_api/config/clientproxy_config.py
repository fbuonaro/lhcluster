#!/usr/bin/python3.6

from pylhcluster import plhc_ext
from pylhcluster.plhc_api.config.actor_config import ClusterActorFactory
from pylhcluster.plhc_api.quickstart import CreateClientProxyParametersBuilder
from pylhcluster.plhc_api.quickstart import CreateEndpointFromDict

class ClientProxyActorFactory( ClusterActorFactory ):
    CLIENTPROXY_PARAM_FIELDS = \
    [
        'selfBrokerMessagingEndpointType',
        'selfBrokerMessagingEndpointBaseAddress',
        'selfClientMessagingEndpointBaseAddress',
        'selfBrokerMessagingEndpointPort',
        'selfClientMessagingEndpointType',
        'selfClientMessagingEndpointPort',
        'controlEndpointBaseAddress',
        'initialBrokerHeartbeatDelayS',
        'selfHeartbeatDelayS',
        'brokerPulseDelayS'
    ]

    TYPE_NAME = "clientproxy"

    def __init__( self, clientProxyConfig ):
        """
        <ClientProxy>:
        {
            ?cluster: <cluster>
            brokerFrontendEndpoint: <string>
        }
        """
        super().__init__( clientProxyConfig.get( 'cluster', {} ) )

        cpFactory = plhc_ext.ClientProxyFactory()
        self.genActor = \
            lambda param : plhc_ext.ClientProxy( cpFactory, param )

        self.type = ClientProxyActorFactory.TYPE_NAME

        brokerFrontendEndpoint = \
            CreateEndpointFromDict( clientProxyConfig[ 'brokerFrontendEndpoint' ] )

        self.parameterBuilder = \
            CreateClientProxyParametersBuilder(
                brokerFrontendEndpoint,
                **{ f : v for f,v in clientProxyConfig.items() \
                    if f in ClientProxyActorFactory.CLIENTPROXY_PARAM_FIELDS } )
