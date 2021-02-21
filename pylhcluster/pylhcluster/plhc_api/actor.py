#!/usr/bin/python3.6

import threading
import time

class GenericClusterActor( threading.Thread ):
    def __init__( self ):
        threading.Thread.__init__( self )
        self.asYouWish = False
        self.allDone = False
        self.pleaseStop = False
        self.thanksForStopping = False

    def Start( self ):
        self.TransitionToRunning()
        self.start()

    def Stop( self ):
        self.TransitionToStopping()
        self.join()
        self.TransitionToStopped()

    def IsRunning( self ):
        return self.asYouWish

    def TransitionToRunning( self ):
        self.asYouWish = True
        self.thanksForStopping = False
        self.allDone = False

    def IsStopping( self ):
        return self.pleaseStop

    def TransitionToStopping( self ):
        self.pleaseStop = True

    def IsStopped( self ):
        return self.thanksForStopping

    def TransitionToStopped( self ):
        self.asYouWish = False
        self.pleaseStop = False
        self.thanksForStopping = True

    def IsDone( self ):
        return self.allDone

    def SignalDone( self ):
        self.allDone = True

    def run( self ):
        raise Exception( "subclass must define run" )

class TimedClusterActor( GenericClusterActor ):
    def __init__( self, actor, stopAfterS ):
        super().__init__()
        self.actor = actor
        self.stopAfterS = stopAfterS
        if( self.stopAfterS <= 0 ):
            raise Exception( "timeout must be greater than 0" )

    def run( self ):
        self.actor.Start() # expected to async
        time.sleep( self.stopAfterS )
        self.actor.Stop()  # expected to be sync

    def GetUnderlyingActor( self ):
        return self.actor

class UntilDoneClusterActor( GenericClusterActor ):
    def __init__( self, actor, timeoutS, allowStop ):
        super().__init__()
        self.actor = actor
        self.timeoutS = timeoutS
        if( self.timeoutS <= 0 ):
            raise Exception( "timeout must be greater than 0" )
        self.allowStop = allowStop

    def run( self ):
        self.actor.Start() # expected to be async

        for sleepCycle in range( self.timeoutS ):
            if self.actor.IsDone():
                break
            if self.allowStop and self.IsStopping():
                break
            time.sleep( 1 )

        self.actor.Stop()  # expected to be sync

        if self.actor.IsDone():
            self.SignalDone()
            
    def GetUnderlyingActor( self ):
        return self.actor
