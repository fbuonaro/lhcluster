#!/usr/bin/python3.6

from pylhcluster import plhc_ext
from pylhcluster.plhc_api import config
from pylhcluster.plhc_api.logging import LoggingSettingsBuilder

def filteredOut( actorType, actorName, filter ):
    ret = True

    if filter is None:
        ret = False # no filter => include everything
    else:
        if actorType in filter:
            actors = filter[ actorType ]
            if actors and ( 'all' in actors or actorName in actors ):
                ret = False # included

    return ret

def createActors(
    clusterName="test",
    loggingBuilder=None,
    loggingDir="./",
    filter=None,
    **factoriesByActorType ):
    actorsByTypeByName = {}

    for actorType, factoriesForType in factoriesByActorType.items():
        actorsForType = {}
        for actorName, actorFactory in factoriesForType.items():
            if( not filteredOut( actorType, actorName, filter ) ):
                actorsForName = actorFactory.CreateActors( \
                    clusterName=clusterName,
                    loggingBuilder=loggingBuilder,
                    loggingDir=loggingDir )
                actorsForType[ actorName ] = actorsForName
        actorsByTypeByName[ actorType ] = actorsForType

    return actorsByTypeByName

def CreateActorsFromClusterConfig( clusterConfig, filter=None ):
    actors = None
    brokersByName = {}
    clientProxiesByName = {}
    loggingBuilder = None
    notifyBrokersByName = {}
    clientsByName = {}
    workersByName = {}


    clusterName = clusterConfig[ 'name' ] # string
    loggingConfig = clusterConfig.get( 'logging', {} )
    initLogging = loggingConfig.get( 'autoInit', False ) # True or False
    loggingDir = loggingConfig.get( 'loggingDirectory', "./" )

    if( config.BrokerActorFactory.TYPE_NAME in clusterConfig ):
        for brokerName, brokerConfig in \
            clusterConfig[ config.BrokerActorFactory.TYPE_NAME ].items():
            brokersByName[ brokerName ] = config.BrokerActorFactory( brokerConfig )

    if( config.ClientProxyActorFactory.TYPE_NAME in clusterConfig ):
        for clientProxyName, clientProxyConfig in \
            clusterConfig[ config.ClientProxyActorFactory.TYPE_NAME ].items():
            clientProxiesByName[ clientProxyName ] = \
                config.ClientProxyActorFactory( clientProxyConfig )

    if( config.NotifyBrokerActorFactory.TYPE_NAME in clusterConfig ):
        for notifyBrokerName, notifyBrokerConfig in \
            clusterConfig[ config.NotifyBrokerActorFactory.TYPE_NAME ].items():
            notifyBrokersByName[ notifyBrokerName ] = \
                config.NotifyBrokerActorFactory( notifyBrokerConfig )

    if( config.WorkerActorFactory.TYPE_NAME in clusterConfig ):
        for workerName, workerConfig in \
            clusterConfig[ config.WorkerActorFactory.TYPE_NAME ].items():
            if( not filteredOut(
                    config.WorkerActorFactory.TYPE_NAME, workerName, filter ) ):
                workersByName[ workerName ] = config.WorkerActorFactory( clusterConfig, workerConfig )

    if( config.ClientActorFactory.TYPE_NAME in clusterConfig ):
        for clientName, clientConfig in \
            clusterConfig[ config.ClientActorFactory.TYPE_NAME ].items():
            if( not filteredOut(
                    config.ClientActorFactory.TYPE_NAME, clientName, filter ) ):
                clientsByName[ clientName ] = config.ClientActorFactory( clusterConfig, clientConfig )

    if( initLogging ):
        loggingBuilder = LoggingSettingsBuilder()
        
    actors = createActors(
        clusterName=clusterName,
        loggingBuilder=loggingBuilder,
        loggingDir=loggingDir,
        filter=filter,
        broker=brokersByName,
        clientproxy=clientProxiesByName,
        notifybroker=notifyBrokersByName,
        worker=workersByName,
        client=clientsByName )

    if( initLogging ):
        plhc_ext.InitializeGenericLoggingFromSettingsString( loggingBuilder.BuildSettings() )

    return actors

def FlattenActorsByTypeByName( actorsByTypeByName ):
    actorsFlat = [ actor for actorType, actorsByName in actorsByTypeByName.items() \
                         for actorName, actors in actorsByName.items() \
                         for actor in actors ]
    return actorsFlat

def CreateActorsFromClusterConfigFlat( clusterConfig, filter=None ):
    actorsByTypeByName = CreateActorsFromClusterConfig( clusterConfig, filter=filter )

    return FlattenActorsByTypeByName( actorsByTypeByName )
