#!/usr/bin/python3.6

import random
import string
import time
import weakref

from pylhcluster import plhc_ext
from pylhcluster import plhc_api

def getRandomUTF8ByteStr():
    strLength = random.randint( 8, 16 )
    chars = []
    for i in range( strLength ):
        chars.append( random.choice( string.ascii_lowercase ) )
    return ''.join( chars ).encode( "utf-8" )

def getRandomId():
    return random.randint( 1, 1000000 )

class NotifyClient( plhc_api.GenericClusterActor ):
    def __init__(
        self,
        plhcClient,
        plhcClientParams,
        clusterConfig,
        implConfig=None,
        instanceNum=0,
        processNum=0,
        threadNum=0 ):
        super().__init__()

        # I think I am done with python
        weakself = weakref.ref( self )
        notifyHandler = plhc_api.SimpleNotifyHandler( \
            lambda ntype, nmsg: weakself().handleNotification( ntype, nmsg ) )

        self.plhcClient = plhcClient

        plhcPublisherEndpoint = \
            plhc_api.CreateEndpointFromDict( implConfig[ 'publisherEndpoint' ] )
        plhcSubscriberParamBuilder = \
            plhc_api.CreateNotifySubscriberParametersBuilder( \
                plhcPublisherEndpoint,
                instanceNum=instanceNum,
                processNum=processNum,
                threadNum=threadNum )
        self.plhcSubscriber = \
            plhc_api.CreateNotifySubscriber(
                plhcSubscriberParamBuilder.Build(),
                plhc_ext.NotificationTypeList(),
                notifyHandler )

        self.numAsyncRequests = implConfig[ 'numAsyncRequests' ]
        self.numSyncRequests = implConfig[ 'numSyncRequests' ]
        self.notificationsReceived = {}
        self.rtype = random.randint( 1, 6 )
        self.syncResponses = plhc_ext.RequestResponseList()

    def handleNotification( self, ntype, msg ):
        print( "NotifyClient: received notification[{}]".format( ntype ) )

        ret = plhc_ext.RequestStatus.Succeeded # will trigger unsub

        if ntype in self.notificationsReceived:
            self.notificationsReceived[ ntype ].append( msg.Duplicate() )
            self.numErrors = self.numErrors + 1
            ret = plhc_ext.RequestStatus.Failed
        else:
            self.notificationsReceived[ ntype ] = [ msg.Duplicate() ]

        return ret

    def run( self ):
        startTimeS = 0.0
        stopTimeS = 0.0
        elapsedTimeMS = 0
        numResponsesOutstanding = 0
        self.numErrors = 0

        self.plhcSubscriber.Start()

        time.sleep( 1 ) # HACK wait a bit to ensure sub connected with some luck
                        # NOT A PROD PATTERN, NOT A GUARANTEE of connected and subbed

        self.plhcClient.Activate()

        numSyncRequestsLeft = self.numSyncRequests
        numAsyncRequestsLeft = self.numAsyncRequests

        startTime = time.perf_counter()
        while( ( numSyncRequestsLeft > 0 ) and ( numAsyncRequestsLeft > 0 ) ):
            sendSyncNext = random.choice( [ True, False ] )
            if( sendSyncNext ):
                self.sendRandomSyncRequest()
                numSyncRequestsLeft = numSyncRequestsLeft - 1
            else:
                self.sendRandomAsyncRequest()
                numAsyncRequestsLeft = numAsyncRequestsLeft - 1

        for _ in range( numSyncRequestsLeft ):
            self.sendRandomSyncRequest()
            
        for _ in range( numAsyncRequestsLeft ):
            self.sendRandomAsyncRequest()

        endTime = time.perf_counter()
        elapsedTimeMS = int( 1000.0 * ( endTime - startTime ) )

        numResponsesOutstanding = self.numSyncRequests - len( self.syncResponses )

        if( numResponsesOutstanding > 0 ):
            numResponsesReceived = len( self.syncResponses )
            timeToWaitMS = elapsedTimeMS

            if( numResponsesOutstanding > numResponsesReceived ):
                waitMultiplier = int( numResponsesOutstanding / numResponsesReceived )
                if waitMultiplier <= 1:
                    waitMultiplier = 2
                timeToWaitMS = timeToWaitMS * waitMultiplier

            print( "NotifyClient.Info: Waiting additional {}ms for {} responses".format( \
                timeToWaitMS,
                numResponsesOutstanding ) )
                
            ret = self.plhcClient.RetrieveResponsesTimeoutMS( \
                self.syncResponses, numResponsesOutstanding, plhc_ext.DelayMS( timeToWaitMS ) ) 

            if( ret != numResponsesOutstanding ):
                print( "NotifyClient.Error: Did not receive expected number of responses[{}]".format( \
                    ret ) )
                self.numErrors = self.numErrors + ( numResponsesOutstanding - ret )

        endTime = time.perf_counter()
        elapsedTimeMS = int( 1000.0 * ( endTime - startTime ) )

        numResponsesOutstanding = self.numAsyncRequests - len( self.notificationsReceived )

        if( numResponsesOutstanding > 0 ):
            numResponsesReceived = len( self.notificationsReceived )
            timeToWaitMS = elapsedTimeMS

            if( numResponsesOutstanding > numResponsesReceived ):
                waitMultiplier = int( numResponsesOutstanding / numResponsesReceived )
                if waitMultiplier <= 1:
                    waitMultiplier = 2
                timeToWaitMS = timeToWaitMS * waitMultiplier

            if timeToWaitMS < 1000:
                timeToWaitMS = 1000

            print( "NotifyClient.Info: Waiting additional {}ms for {} notifications".format( \
                timeToWaitMS,
                numResponsesOutstanding ) )

            time.sleep( int( timeToWaitMS / 1000 ) )

            numResponsesOutstanding = self.numAsyncRequests - len( self.notificationsReceived )
            if( numResponsesOutstanding > 0 ):
                print( "NotifyClient.Error: some notifications still outstanding[{}]".format( \
                    numResponsesOutstanding ) )

        self.plhcClient.Deactivate()
        self.plhcSubscriber.Stop()

        print( "Run Results:" )
        print( "NumSyncRequests     {}".format( self.numSyncRequests ) )
        print( "NumSyncResponses    {}".format( len( self.syncResponses ) ) )
        print( "NumAsyncRequests    {}".format( self.numAsyncRequests ) )
        print( "NumNotifications    {}".format( len( self.notificationsReceived ) ) )
        print( "NumErrors           {}".format( self.numErrors ) )

        self.SignalDone()

    def sendRandomSyncRequest( self ):
        ret = 0
        id = getRandomId()
        zmsg = plhc_ext.PyZMQMessage()
        zmsg.AppendBytes( bytes( getRandomUTF8ByteStr() ) )

        ret = self.plhcClient.SendSynchronousRequest( self.rtype, id, zmsg )
        if( ret != 0 ):
            print( "NotifyClient.Error: failed to send sync request[{}]".format( ret ) )
            self.numErrors = self.numErrors + 1

        ret = self.plhcClient.RetrieveResponsesTimeoutMS( \
            self.syncResponses, 1, plhc_ext.DelayMS( 10 ) ) 
        if( ret != 1 ):
            print( "NotifyClient.Warning: failed to retrieve response in 60ms[{}|{}]".format( self.rtype, id ) )

    def sendRandomAsyncRequest( self ):
        ret = 0
        id = getRandomId()
        msg = bytes( getRandomUTF8ByteStr() )
        zmsg = plhc_ext.PyZMQMessage()
        zmsg.AppendBytes( msg )

        notifyType = "{}-{}".format( self.rtype, id )

        print( "NotifyClient: subscribing[{}]".format( notifyType ) )

        subscriptions = [ notifyType ]
        subscriptions = plhc_api.CreateNotificationTypeListFromPyIterable( subscriptions )
        ret = self.plhcSubscriber.Subscribe( subscriptions, plhc_ext.DelayMS( 50 ) )
        if( ret != 0 ):
            print( "NotifyClient.Error: failed to sub to {} in timeout[{}]".format( subscriptions, ret ) )
            self.numErrors = self.numErrors + 1

        ret = self.plhcClient.SendAsynchronousRequest( self.rtype, id, zmsg )
        if( ret != 0 ):
            print( "NotifyClient.Error: failed to send async request[{}]".format( ret ) )
            self.numErrors = self.numErrors + 1
