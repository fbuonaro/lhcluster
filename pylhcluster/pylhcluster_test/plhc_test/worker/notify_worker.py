#!/usr/bin/python3.6

import time

from pylhcluster import plhc_api
from pylhcluster import plhc_ext

class NotifyRequestHandler( plhc_api.SimpleRequestHandler ):
    def __init__(
        self,
        workerParams,
        clusterConfig,
        implConfig=None,
        instanceNum=None,
        processNum=None,
        threadNum=None ):
        super( NotifyRequestHandler, self ).__init__()
        self.receivedRequests = {}
        self.failedRequests = {}

        plhcSubscriberEndpoint = \
            plhc_api.CreateEndpointFromDict( implConfig[ 'subscriberEndpoint' ] )
        plhcPublisherParamBuilder = \
            plhc_api.CreateNotifyPublisherParametersBuilder(
                proxyEndpoint=plhcSubscriberEndpoint )
        self.plhcPublisher = plhc_api.CreateNotifyPublisher( plhcPublisherParamBuilder.Build() )
        self.numRequestsReceived = 0

    def Process( self, vfs, requestType, requestId, pyZMQMessage ):
        
        print( "NotifyRequestHandler.Process: receieved message[{}|{}|{}]".format( \
            vfs, requestType, requestId ) )

        msgRespBytes = []
        msgBytes = pyZMQMessage.BytesAtFront()
        msgStr = msgBytes.tobytes().decode( "utf-8" )
        sortedMsgStr = sorted( msgStr )
        sortedMsgBytes = str( sortedMsgStr ).encode( "utf-8" )

        print( "NotifyRequestHandler.Process: processed message[{}|{}]".format( \
            msgStr, sortedMsgStr ) )

        self.receivedRequests[ ( requestType, requestId ) ] = ( vfs, pyZMQMessage.Duplicate() )
        self.numRequestsReceived = self.numRequestsReceived + 1

        if self.numRequestsReceived % 100 == 0:
            time.sleep( 1 )

        if( vfs.flags & plhc_ext.MessageFlag.Asynchronous ):
            print( "NotifyRequestHandler.Process: sending async response" )
            msgResp = plhc_ext.PyZMQMessage()
            msgResp.PrependBytes( sortedMsgBytes )
            notifyType = "{}-{}".format( requestType, requestId )
            print( "NotifyRequestHandler.Process: sending notification[{}]".format( notifyType ) )
            if( self.plhcPublisher.PublishTypeIdMessage( notifyType, requestId, msgResp ) != 0 ):
                print( "NotifyRequestHandler.Process: failed sending notification[{}]".format( \
                    notifyType ) )
            if( self.plhcPublisher.PublishTypeIdMessage( "TEST", requestId, msgResp ) != 0 ):
                print( "NotifyRequestHandler.Process: failed sending TEST notification" )
        else:
            print( "NotifyRequestHandler.Process: sending sync response" )
            msgRespBytes.append( sortedMsgBytes )

        return ( plhc_ext.RequestStatus.Succeeded,  msgRespBytes )

    def ErrorProcess( self, e, vfs, requestType, requestId, pyZMQMessage ):
        print( "ERROR: NotifyRequestHandler failed to process request[{}|{}]".format(
            requestType, requestId ) )
        self.receivedRequests[ ( requestType, requestId ) ] = ( vfs, pyZMQMessage.Duplicate() )
        pass

    def SupportedRequestTypes( self ):
        return [ 1, 2, 3, 4, 5, 6 ]
