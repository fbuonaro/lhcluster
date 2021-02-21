#!/usr/bin/python3.6

from pylhcluster import plhc_ext

def CreateNotificationTypeListFromPyIterable( notificationTypes ):
    notificationTypeList = plhc_ext.NotificationTypeList()
    notificationTypeList[:] = notificationTypes
    return notificationTypeList

def CreateRequestTypeListFromPyIterable( requestTypes ):
    requestTypeList = plhc_ext.RequestTypeList()
    requestTypeList[:] = requestTypes
    return requestTypeList
