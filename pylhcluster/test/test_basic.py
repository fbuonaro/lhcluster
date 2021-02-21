#!/usr/bin/python3.6

import pytest
import time

from pylhcluster import plhc_api
from pylhcluster import plhc_ext

@pytest.fixture
def testClusterConfig():
    testClusterConfig = {}
    testClusterConfig[ 'name' ] = 'test_basic'

    testClusterConfig[ plhc_api.BrokerActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ] = {}

    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ] = {}
    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ][ 'brokerFrontendEndpoint' ] = { \
        "endpointType": plhc_ext.EndpointType.InterThread,
        "endpointBaseAddress": "quickstart.broker_frontend_1_1_1.zitc"
    }

    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ][ 'timeoutS' ] = 1
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ][ 'allowStop' ] = True
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'impl' ] = { \
        'module': 'pylhcluster_test.plhc_test',
        'class': 'BasicClient'
    }
    testClusterConfig[ 'client' ][ 'basic_client' ][ 'cpClientMessagingEndpoint' ] = { \
        "endpointType": plhc_ext.EndpointType.InterThread,
        "endpointBaseAddress": "quickstart.clientproxy_client_1_1_1.zitc"
    }

    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ] = {}
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'impl' ] = { \
        'module': 'pylhcluster_test.plhc_test',
        'class': 'BasicRequestHandler'
    }
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'brokerBackendEndpoint' ] = { \
        "endpointType": plhc_ext.EndpointType.InterThread,
        "endpointBaseAddress": "quickstart.broker_backend_1_1_1.zitc"
    }

    return testClusterConfig


def test_ClientBrokerWorker( testClusterConfig ):
    actorsByTypeByName = plhc_api.CreateActorsFromClusterConfig( testClusterConfig )

    for eachActor in actorsByTypeByName[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ]:
        eachActor.Start()

    time.sleep( 1 )

    for eachActor in actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ]:
        eachActor.Stop()
        assert( eachActor.IsDone() )
    for eachActor in actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ]:
        eachActor.Stop()

    clientActor = actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ]\
                                    [ 'basic_client' ][ 0 ].GetUnderlyingActor()
    workerActor = actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ]\
                                    [ 'basic_worker' ][ 0 ]
    requestHandlerCallback = workerActor.GetRequestHandler() # hack

    assert( clientActor.IsDone() )
    assert( 6 == len( clientActor.responses ) )
    assert( 6 == len( requestHandlerCallback.receivedRequests ) )
    for eachRequestType in [ 1, 2, 3 ]:
        reqKey1 = ( eachRequestType, eachRequestType )
        reqKey2 = ( eachRequestType, 10 * eachRequestType )
        req1 = requestHandlerCallback.receivedRequests.get( reqKey1, None )
        req2 = requestHandlerCallback.receivedRequests.get( reqKey2, None )
        assert( req1 is not None )
        assert( req2 is not None )
        assert( 0 == req1[ 1 ].TotalNumberOfFrames() )
        assert( 0 == req2[ 1 ].TotalNumberOfFrames() )
        assert( None == req1[ 1 ].NextBytes() )
        assert( None == req2[ 1 ].NextBytes() )
        assert( plhc_ext.MessageFlag.Nil == req1[ 0 ].flags )
        assert( plhc_ext.MessageFlag.Nil == req2[ 0 ].flags )
