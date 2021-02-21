#!/usr/bin/python3.6

from pylhcluster import plhc_ext
from pylhcluster.plhc_api.container import CreateRequestTypeListFromPyIterable

class SimpleRequestHandler( plhc_ext.PyIRequestHandler ):
    def __init__( self ):
        super( SimpleRequestHandler, self ).__init__()
        self.lastError = None
        self.requestTypeList = []

    def PyProcess( self, vfs, requestType, requestId, pyZMQMessage ):
        try:
            ret = plhc_ext.PyProcessorResult()
            status, responseFrames = \
                self.Process( vfs, requestType, requestId, pyZMQMessage )

            ret.SetStatus( status )
            responseMessage = ret.GetPyZMQMessage()
            for eachResponseFrame in responseFrames:
                responseMessage.AppendBytes( eachResponseFrame )

            return ret
        except Exception as e:
            self.lastError = str( e )
            ret = plhc_ext.PyProcessorResult()
            ret.SetStatus( plhc_ext.RequestStatus.FatalError ) # shut down

        return ret

    def PySupportedRequestTypes( self ):
        # TODO - seriously fix this for the sake of usability
        #        the following results in a fairly serious error
        #        I think it is the type of PySupportedRequestTypes in
        #        native
        # requestTypeList = CreateRequestTypeListFromPyIterable( \
        #     self.SupportedRequestTypes() )
        # return requestTypeList
        pyRequestTypes = self.SupportedRequestTypes()
        self.requestTypeList = CreateRequestTypeListFromPyIterable( \
            pyRequestTypes )
        return self.requestTypeList

    def GetLastError( self ):
        return self.lastError

    # def Process( self, vfs, requestType, requestId, pyZMQMessage ):
    #     ...

    # def ErrorProcess( self, e, vfs, requestType, requestId, pyZMQMessage ):
    #     ...

    # def SupportedRequestTypes( self ):
    #     ...

class SimpleRequestHandlerCB( SimpleRequestHandler ):
    def __init__( self, requestTypes, callback ):
        super( SimpleRequestHandlerCB, self ).__init__()
        self.requestTypes = CreateRequestTypeListFromPyIterable( \
            requestTypes )
        self.callback = callback

    def Process( self, vfs, requestType, requestId, pyZMQMessage ):
        return self.callback( vfs, requestType, requestId, pyZMQMessage )

    def ErrorProcess( self, e, vfs, requestType, requestId, pyZMQMessage ):
        pass

    def SupportedRequestTypes( self ):
        return self.requestType
