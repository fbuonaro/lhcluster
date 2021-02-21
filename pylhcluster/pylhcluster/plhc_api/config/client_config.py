#!/usr/bin/python3.6

from importlib import import_module

from pylhcluster.plhc_api.config.actor_config import ClusterActorFactory
from pylhcluster.plhc_api.quickstart import CreateClientParametersBuilder
from pylhcluster.plhc_api.quickstart import CreateClientFromParameters
from pylhcluster.plhc_api.quickstart import CreateEndpointFromDict

class ClientActorFactory( ClusterActorFactory ):
    CLIENT_PARAM_FIELDS = \
    [
        'clientEndpointType',
        'clientEndpointBaseAddress',
        'clientEndpointPort'
    ]

    TYPE_NAME = "client"

    def CreateClientFromParameters( self, clientParams ):
        client = CreateClientFromParameters( clientParams )
        clientActorImpl = \
            self.implClass( \
                client,
                clientParams,
                self.clusterConfig,
                implConfig=self.implConfig,
                instanceNum=self.instance,
                processNum=self.process,
                threadNum=self.thread )

        return clientActorImpl
     

    def __init__( self, clusterConfig, clientConfig ):
        """
        <Client>:
        {
            ?cluster: <Cluster>
            cpClientMessagingEndpoint : <string>
            impl : 
            {
                module: <string | path to module containing class>
                class: <string | name of class in module>
            }
            ?implConfig : <dict | custom config for impl class>
        }

        The impl must be an Actor subclass constructable as follows:
            ClientActorImpl(
                clientHandle,
                clientParameters,
                clusterConfig,
                implConfig=None,
                instanceNum=None,
                processNum=None,
                threadNum=None )
            
        """
        super().__init__( clientConfig.get( 'cluster', {} ) )

        cpClientMessagingEndpoint = \
            CreateEndpointFromDict( clientConfig[ 'cpClientMessagingEndpoint' ] )

        self.parameterBuilder = CreateClientParametersBuilder(
            cpClientMessagingEndpoint,
            **{ f : v for f,v in clientConfig.items() \
                if f in ClientActorFactory.CLIENT_PARAM_FIELDS } )

        self.implClass = \
            getattr(
                import_module( clientConfig[ 'impl' ][ 'module' ] ),
                clientConfig[ 'impl' ][ 'class' ] )

        self.clusterConfig = clusterConfig
        self.implConfig = clientConfig.get( 'implConfig', None )

        self.type = ClientActorFactory.TYPE_NAME

        self.genActor = self.CreateClientFromParameters
