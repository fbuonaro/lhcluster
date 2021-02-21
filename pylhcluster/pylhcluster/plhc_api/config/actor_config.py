#!/usr/bin/python3.6

from pylhcluster.plhc_api.actor import TimedClusterActor
from pylhcluster.plhc_api.actor import UntilDoneClusterActor

class ClusterActorFactory():
    AUTOCONFIGLOGGING = \
    {
        'broker': True,
        'clientproxy': True,
        'worker': True
    }

    CLUSTER_ACTOR_PARAM_FIELDS_WDFLT = \
    [
        ( 'stopAfterS', None ),
        ( 'timeoutS', None ),
        ( 'allowStop', False ),
        ( 'numInstances', 1 ),  # for higher level scripts, not used here
        ( 'numProcesses', 1 ),  # for higher level scripts, not used here
        ( 'numThreads', 1 ),    # how many actors to create
        ( 'instance', 1 ),      # which machine ?
        ( 'process', 1 ),       # which process on that machine ?
        ( 'thread', 1 )         # which thread in that process
    ]

    def __init__( self, clusterConfig ):
        """
        <Cluster>:
        {
            ?name : value for name in CLUSTER_ACTOR_PARAM_FIELDS_WDFLT
        }
        """
        # set in subclass
        self.parameterBuilder = None
        # set in subclass
        # genActor : something which accepts one parameter and creates something providing
        #            a Start, Stop (i.e. a GenericClusterActor subclass)
        #            can be a class a function returning a class, etc
        #            the passed parameter will be the result of ParameterBuilder().Build()
        self.genActor = None
        # set in subclass
        self.type = None
        for fieldName, defaultValue in ClusterActorFactory.CLUSTER_ACTOR_PARAM_FIELDS_WDFLT:
            setattr(
                self,
                fieldName,
                clusterConfig.get( fieldName, defaultValue ) )

    def ParameterBuilder( self ):
        return self.parameterBuilder

    def StopAfterS( self ):
        return self.stopAfterS

    def TimeoutS( self ):
        return self.timeoutS

    def GetType( self ):
        return self.type

    def CreateActors(
        self,
        clusterName="",
        loggingBuilder=None,
        loggingDir="./" ):
        actors = []
        self.ParameterBuilder() \
            .SetInstance( self.instance ) \
            .SetProcess( self.process )
        # TODO - warn if multiple threads using same TCP port
        #        have subclasses define GetEndpointTypes() to return all
        #        endpoint types used by this actor factory instance
        for thread in range( 1, self.numThreads+1 ):
            self.thread = thread # TODO - remove this HACK into client factory :(
            self.ParameterBuilder().SetThread( thread )
            params = self.ParameterBuilder().Build()
            if( loggingBuilder is not None ):
                if( ClusterActorFactory.AUTOCONFIGLOGGING.get( self.GetType(), False ) ):
                    loggingBuilder.AddSettingsSegment(
                        self.GetType(),
                        clusterName,
                        name=params.GetControlEndpoint(),
                        directory=loggingDir )

            actor = self.genActor( params )

            if( self.stopAfterS is not None ):
                actor = TimedClusterActor( actor, self.stopAfterS )
            elif( self.timeoutS is not None ):
                actor = \
                    UntilDoneClusterActor( \
                        actor, self.timeoutS, self.allowStop )

            actors.append( actor )

        return actors
