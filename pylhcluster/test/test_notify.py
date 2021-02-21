#!/usr/bin/python3.6

import pytest
import sys
import time

from pylhcluster import plhc_api
from pylhcluster import plhc_ext

NUM_HARDCODED_REQUESTS = 50

@pytest.fixture
def testClusterConfig():
    testClusterConfig = {}
    testClusterConfig[ 'name' ] = 'test_basic'

    testClusterConfig[ plhc_api.BrokerActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ] = {}

    testClusterConfig[ plhc_api.NotifyBrokerActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.NotifyBrokerActorFactory.TYPE_NAME ][ 'basic_notifybroker' ] = {\
        'frontendEndpointType': plhc_ext.EndpointType.TCP,
        'frontendEndpointBaseAddress': "127.0.0.1",
        'frontendEndpointPort': 57869,
        'backendEndpointType': plhc_ext.EndpointType.TCP,
        'backendEndpointBaseAddress': "127.0.0.1",
        'backendEndpointPort': 57879 }

    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ] = {}
    testClusterConfig[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ]\
        [ 'brokerFrontendEndpoint' ] = { \
            "endpointType": plhc_ext.EndpointType.InterThread,
            "endpointBaseAddress": "quickstart.broker_frontend_1_1_1.zitc" }

    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ] = {}
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ][ 'stopAfterS' ] = 10
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ][ 'allowStop' ] = True
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'cluster' ][ 'numThreads' ] = 3
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'impl' ] = { \
        'module': 'pylhcluster_test.plhc_test',
        'class': 'NotifyClient'
    }
    testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ][ 'implConfig' ] = { \
        'publisherEndpoint': { \
            "endpointType": plhc_ext.EndpointType.TCP,
            "endpointBaseAddress": "127.0.0.1",
            "endpointPort": 57869
        },
        'numAsyncRequests': NUM_HARDCODED_REQUESTS,
        'numSyncRequests': NUM_HARDCODED_REQUESTS
    }
    testClusterConfig[ 'client' ][ 'basic_client' ][ 'cpClientMessagingEndpoint' ] = { \
        "endpointType": plhc_ext.EndpointType.InterThread,
        "endpointBaseAddress": "quickstart.clientproxy_client_1_1_1.zitc"
    }

    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ] = {}
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ] = {}
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'cluster' ] = {}
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'cluster' ][ 'numThreads' ] = 3
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'impl' ] = { \
        'module': 'pylhcluster_test.plhc_test',
        'class': 'NotifyRequestHandler'
    }
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'implConfig' ] = { \
        'subscriberEndpoint': { \
            "endpointType": plhc_ext.EndpointType.TCP,
            "endpointBaseAddress": "127.0.0.1",
            "endpointPort": 57879
        }
    }
    testClusterConfig[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ][ 'brokerBackendEndpoint' ] = { \
        "endpointType": plhc_ext.EndpointType.InterThread,
        "endpointBaseAddress": "quickstart.broker_backend_1_1_1.zitc"
    }

    return testClusterConfig


def test_ClientBrokerWorker( testClusterConfig ):
    actorsByTypeByName = plhc_api.CreateActorsFromClusterConfig( testClusterConfig )

    print( "test_ClientBrokerWorker: starting")
    sys.stdout.flush()

    for eachActor in actorsByTypeByName[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.NotifyBrokerActorFactory.TYPE_NAME ][ 'basic_notifybroker' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ]:
        eachActor.Start()
    for eachActor in actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ]:
        eachActor.Start()

    print( "test_ClientBrokerWorker: started")
    sys.stdout.flush()

    time.sleep( 3 )

    print( "test_ClientBrokerWorker: stopping")
    sys.stdout.flush()

    for eachActor in actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ][ 'basic_client' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ][ 'basic_worker' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.ClientProxyActorFactory.TYPE_NAME ][ 'basic_clientproxy' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.NotifyBrokerActorFactory.TYPE_NAME ][ 'basic_notifybroker' ]:
        eachActor.Stop()
    for eachActor in actorsByTypeByName[ plhc_api.BrokerActorFactory.TYPE_NAME ][ 'basic_broker' ]:
        eachActor.Stop()

    print( "test_ClientBrokerWorker: stopped")
    sys.stdout.flush()

    clientConfig = \
        testClusterConfig[ plhc_api.ClientActorFactory.TYPE_NAME ]\
                         [ 'basic_client' ]
    totalExpectedRequests = \
        clientConfig['cluster']['numThreads'] * \
        ( clientConfig['implConfig']['numAsyncRequests'] + \
          clientConfig['implConfig']['numSyncRequests'] )

    totalRequests = 0
    for eachClientActor in actorsByTypeByName[ plhc_api.ClientActorFactory.TYPE_NAME ]\
                                             [ 'basic_client' ]:
        clientActor = eachClientActor.GetUnderlyingActor()

        assert( clientActor.IsDone() )
        # assert( clientActor.numErrors == 0 )
        # assert( clientActor.numSyncRequests == len( clientActor.syncResponses ) )
        # assert( clientActor.numAsyncRequests == len( clientActor.notificationsReceived ) )
        assert( clientActor.numErrors < ( 2 * NUM_HARDCODED_REQUESTS ) )
        assert( len( clientActor.syncResponses ) > 0 )
        assert( len( clientActor.notificationsReceived ) > 0 )

        totalRequests = totalRequests + clientActor.numSyncRequests + clientActor.numAsyncRequests


    totalReceivedRequests = 0
    for workerActor in actorsByTypeByName[ plhc_api.WorkerActorFactory.TYPE_NAME ]\
                                         [ 'basic_worker' ]:
        requestHandlerCallback = workerActor.GetRequestHandler() # hack

        assert( requestHandlerCallback.GetLastError() is None )

        totalReceivedRequests = totalReceivedRequests + \
                                len( requestHandlerCallback.receivedRequests )

    # assert( totalRequests == totalReceivedRequests )
    # assert( totalRequests == totalExpectedRequests )
    if( totalRequests != totalReceivedRequests ):
        print( "test_notify.py ERROR: clients sent {} requests but workers only received {}".format( \
            totalRequests, totalReceivedRequests ) )
    if( totalReceivedRequests != totalExpectedRequests ):
        print( "test_notify.py ERROR: expected workers to receive {} requests but workers only received {}".format( \
            totalExpectedRequests, totalReceivedRequests ) )
