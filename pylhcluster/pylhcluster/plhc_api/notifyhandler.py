#!/usr/bin/python3.6

from pylhcluster import plhc_ext

class SimpleNotifyHandler( plhc_ext.PyINotifyHandler ):
    def __init__( self, notifyHandlerCallback ):
        super( SimpleNotifyHandler, self ).__init__()
        self.notifyHandlerCallback = notifyHandlerCallback

    def PyProcess( self, notifyType, pyZMQMessage ):
        try:
            ret = self.notifyHandlerCallback( notifyType, pyZMQMessage )

            return ret
        except Exception as e:
            return plhc_ext.RequestStatus.FatalError # stop subscriber
