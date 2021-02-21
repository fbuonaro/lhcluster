from pylhcluster import plhc_ext
from pylhcluster import plhc_api
from pylhcluster.plhc_api import ClusterActorFactory
from pylhcluster.plhc_api import BrokerActorFactory
from pylhcluster.plhc_api import ClientProxyActorFactory
from pylhcluster.plhc_api import NotifyBrokerActorFactory

def checkActorFactoryAgainstConfig( actorFactory, actorConfig ):
    for fieldName, fieldValue in actorConfig.items():
        assert( fieldValue == getattr( actorFactory, fieldName ) )

def test_config_actor_defaults():
    defaultClusterActorFactory = ClusterActorFactory( {} )
    for fieldName, defaultValue in ClusterActorFactory.CLUSTER_ACTOR_PARAM_FIELDS_WDFLT:
        assert( defaultValue == getattr( defaultClusterActorFactory, fieldName ) )

def createClusterActorConfigDict():
    actorConfig = \
    {
        'stopAfterS': 300,
        'numProcesses': 10,
        'numThreads': 3,
        'process': 2,
    }

    return actorConfig

def test_config_actor():
    actorConfig = createClusterActorConfigDict()
    actorFactory = ClusterActorFactory( actorConfig )
    checkActorFactoryAgainstConfig( actorFactory, actorConfig )
    assert( actorFactory.numInstances == 1 )
    assert( actorFactory.instance == 1 )
    assert( actorFactory.thread == 1 )

def createBrokerConfig():
    brokerConfig = \
    {
        'frontendEndpointType': plhc_ext.EndpointType.TCP,
        'frontendEndpointBaseAddress': "127.0.0.1",
        'frontendEndpointPort': 10000,
        'backendEndpointType': plhc_ext.EndpointType.InterProcess,
        'backendEndpointBaseAddress': "test_broker_config_be",
        'controlEndpointBaseAddress': "test_broker_config_control",
        'selfHeartbeatDelayS': 3,
        'minWorkerHeartbeatDelayS': 15,
        'workerPulseDelayS': 15,
        'cluster': createClusterActorConfigDict()
    }

    return brokerConfig

def test_config_broker():
    brokerConfig = createBrokerConfig()
    brokerFactory = BrokerActorFactory( brokerConfig )

    checkActorFactoryAgainstConfig( brokerFactory, brokerConfig[ 'cluster' ] )
    assert( 'broker' == brokerFactory.GetType() )

    brokerActors = brokerFactory.CreateActors()

    assert( 3 == len( brokerActors ) )

    print( brokerActors )
    print( type( brokerActors ) )

    for eachActor in brokerActors:
        assert( type( eachActor ) == plhc_api.TimedClusterActor )
        assert( type( eachActor.actor ) == plhc_ext.Broker )
        assert( eachActor.stopAfterS == brokerFactory.StopAfterS() )


def createClientProxyConfig():
    clientProxyConfig = \
    {
        'selfBrokerMessagingEndpointType': plhc_ext.EndpointType.InterThread,
        'selfBrokerMessagingEndpointBaseAddress': "test_cp_config_bm",
        'selfClientMessagingEndpointType': plhc_ext.EndpointType.InterProcess,
        'selfClientMessagingEndpointBaseAddress': "test_cp_config_cm",
        'controlEndpointBaseAddress': "test_cp_config_control",
        'initialBrokerHeartbeatDelayS': 15,
        'selfHeartbeatDelayS': 10,
        'brokerPulseDelayS': 15
    }

    return clientProxyConfig

def test_config_clientproxy():
    clientProxyConfig = createClientProxyConfig()
    clientProxyConfig[ 'cluster' ] = {}
    clientProxyConfig[ 'cluster' ][ 'timeoutS' ] = 1
    clientProxyConfig[ 'brokerFrontendEndpoint' ] = \
    {
        "endpointType" : plhc_ext.EndpointType.InterProcess,
        "endpointBaseAddress": "test_cp_broker_fe"
    }

    clientProxyFactory = ClientProxyActorFactory( clientProxyConfig )
    assert( "clientproxy" == clientProxyFactory.GetType() )

    brokerFrontendEndpoint = \
        clientProxyFactory.ParameterBuilder().Build().GetBrokerMessagingEndpoint()
    assert( plhc_ext.EndpointType.InterProcess == brokerFrontendEndpoint.type() )
    assert( "ipc://test_cp_broker_fe" == brokerFrontendEndpoint.path() )

    clientProxyActors = clientProxyFactory.CreateActors()

    assert( 1 == len( clientProxyActors ) )

    for eachActor in clientProxyActors:
        assert( type( eachActor ) == plhc_api.UntilDoneClusterActor )
        assert( type( eachActor.actor ) == plhc_ext.ClientProxy )
        assert( eachActor.timeoutS == clientProxyFactory.TimeoutS() )

def createNotifyBrokerConfig():
    notifyBrokerConfig = \
    {
        'frontendEndpointType': plhc_ext.EndpointType.InterThread,
        'frontendEndpointBaseAddress': "test_nb_fe_config",
        'backendEndpointType': plhc_ext.EndpointType.InterProcess,
        'backendEndpointBaseAddress': "test_broker_config",
        'backendEndpointPort': 30000,
        'cluster': createClusterActorConfigDict()
    }

    return notifyBrokerConfig
    
def test_config_notifybroker():
    notifyBrokerConfig = createNotifyBrokerConfig()
    notifyBrokerConfig[ 'cluster' ][ 'numThreads' ] = 1
    notifyBrokerConfig[ 'cluster' ][ 'stopAfterS' ] = 1
    notifyBrokerFactory = NotifyBrokerActorFactory( notifyBrokerConfig )

    checkActorFactoryAgainstConfig( notifyBrokerFactory, notifyBrokerConfig[ 'cluster' ] )
    assert( 'notifybroker' == notifyBrokerFactory.GetType() )

    notifyBrokerActors = notifyBrokerFactory.CreateActors()

    assert( 1 == len( notifyBrokerActors ) )

    for eachActor in notifyBrokerActors:
        assert( type( eachActor ) == plhc_api.TimedClusterActor )
        assert( type( eachActor.actor ) == plhc_ext.NotifyBroker )
        assert( eachActor.stopAfterS == notifyBrokerFactory.StopAfterS() )

def test_config_client():
    # TODO
    pass

def test_config_worker():
    # TODO
    pass
