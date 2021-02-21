#!/usr/bin/python3.6

from pylhcluster import plhc_api
from pylhcluster import plhc_ext

class BasicRequestHandler( plhc_api.SimpleRequestHandler ):
    def __init__(
        self,
        workerParams,
        clusterConfig,
        implConfig=None,
        instanceNum=None,
        processNum=None,
        threadNum=None ):
        super( BasicRequestHandler, self ).__init__()
        self.receivedRequests = {}

    def Process( self, vfs, requestType, requestId, pyZMQMessage ):
        # Duplicate must be called because callback does not own the pyZMQMessage by default
        self.receivedRequests[ ( requestType, requestId ) ] = ( vfs, pyZMQMessage.Duplicate() )
        return ( plhc_ext.RequestStatus.Succeeded, [] )

    def ErrorProcess( self, e, vfs, requestType, requestId, pyZMQMessage ):
        pass

    def SupportedRequestTypes( self ):
        return [ 1, 2, 3 ]
