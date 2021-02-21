#!/usr/bin/python3.6

import time

from pylhcluster import plhc_ext
from pylhcluster import plhc_api

class BasicClient( plhc_api.GenericClusterActor ):
    def __init__(
        self,
        plhcClient,
        plhcClientParams,
        clusterConfig,
        implConfig={},
        instanceNum=0,
        processNum=0,
        threadNum=0,
        notifyBrokerActorFactoriesByName={} ):
        super().__init__()

        self.plhcClient = plhcClient

        self.responses = plhc_ext.RequestResponseList()
        self.numRetrieved = 0

    def run( self ):
        self.plhcClient.Activate()

        self.plhcClient.SendSynchronousRequestEmpty( 1, 1 )
        self.plhcClient.SendSynchronousRequestEmpty( 1, 10 )
        self.plhcClient.SendSynchronousRequestEmpty( 2, 2 )
        self.plhcClient.SendSynchronousRequestEmpty( 2, 20 )
        self.plhcClient.SendSynchronousRequestEmpty( 3, 3 )
        self.plhcClient.SendSynchronousRequestEmpty( 3, 30 )

        self.numRetrieved = self.plhcClient.RetrieveResponsesTimeoutMS( \
            self.responses, 6, plhc_ext.DelayMS( 10 ) )

        self.plhcClient.Deactivate()

        self.SignalDone()
