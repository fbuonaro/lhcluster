#!/usr/bin/python3.6

import os
import pytest
import sys
import threading
import time

from pylhcluster import plhc_api
from pylhcluster import plhc_ext

class AggregatingRequestHandlerCallback:
    def __init__( self ):
        self.receivedRequests = {}

    def __call__( self, vfs, requestType, requestId, pyZMQMessage ):
        # Duplicate must be called because callback does not own the pyZMQMessage by default
        self.receivedRequests[ ( requestType, requestId ) ] = ( vfs, pyZMQMessage.Duplicate() )
        return ( plhc_ext.RequestStatus.Succeeded, [] )

class BasicParameterBuilders():
    def __init__( self, bpb=None, cppb=None, cpb=None, wpb=None ):
        self.bpb = bpb if bpb is not None else \
            plhc_api.CreateBrokerParametersBuilder()

        self.cppb = cppb if cppb is not None else \
            plhc_api.CreateClientProxyParametersBuilder( \
                self.bpb.Build().GetFrontendMessagingEndpoint() )

        self.cpb = cpb if cpb is not None else \
            plhc_api.CreateClientParametersBuilder( \
                self.cppb.Build().GetSelfClientMessagingEndpoint() )

        self.wpb = wpb if wpb is not None else \
            plhc_api.CreateWorkerParametersBuilder( \
                self.bpb.Build().GetBackendMessagingEndpoint() )

@pytest.fixture
def basicParameters():
    return BasicParameterBuilders();

def test_ClientBrokerWorker( basicParameters ):
    broker = plhc_api.CreateBrokerFromParameters( basicParameters.bpb.Build() )
    client = plhc_api.CreateClientFromParameters( basicParameters.cpb.Build() )
    clientProxy = plhc_api.CreateClientProxyFromParameters( basicParameters.cppb.Build() )
    requestHandlerCallback = AggregatingRequestHandlerCallback()
    requestHandler = plhc_api.SimpleRequestHandlerCB( \
        [ 1, 2, 3 ], requestHandlerCallback )
    worker = plhc_api.CreateWorkerFromParameters( basicParameters.wpb.Build(), requestHandler )

    broker.Start()
    clientProxy.Start()
    worker.Start()

    time.sleep( 1 )

    client.Activate()

    responses = plhc_ext.RequestResponseList()
    numRetrieved = 0

    client.SendSynchronousRequestEmpty( 1, 1 )
    client.SendSynchronousRequestEmpty( 1, 10 )
    client.SendSynchronousRequestEmpty( 2, 2 )
    client.SendSynchronousRequestEmpty( 2, 20 )
    client.SendSynchronousRequestEmpty( 3, 3 )
    client.SendSynchronousRequestEmpty( 3, 30 )

    numRetrieved = client.RetrieveResponsesTimeoutMS( responses, 6, plhc_ext.DelayMS( 1000 ) )

    client.Deactivate()

    broker.Stop()
    clientProxy.Stop()
    worker.Stop()

    print( requestHandlerCallback.receivedRequests )
    assert( 6 == len( responses ) )
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
