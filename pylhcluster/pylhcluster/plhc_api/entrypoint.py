#!/usr/bin/python3.6

from pylhcluster.plhc_api.create_actors import CreateActorsFromClusterConfigFlat
from pylhcluster.plhc_ext import EndpointType

import json
import time

def clusterHook( objectPairs ):
    d = {}

    for k, v in objectPairs:
        if isinstance( v, str ):
            if v.startswith( "EndpointType." ):
                if v == "EndpointType.TCP":
                    d[ k ] = EndpointType.TCP
                elif v == "EndpointType.InterThread":
                    d[ k ] = EndpointType.InterThread
                elif v == "EndpointType.InterProcess":
                    d[ k ] = EndpointType.InterProcess
                else:
                    d[ k ] = v
            else:
                d[ k ] = v
        else:
            d[ k ] = v

    return d

def PyLHClusterRun( clusterConfig, filters ):
    actors = CreateActorsFromClusterConfigFlat( clusterConfig, filters )

    for eachActor in actors:
        eachActor.Start()

    time.sleep( 1 )

    for eachActor in actors:
        eachActor.Stop()

    return 0

def PyLHClusterMain( argv ):
    # main.py
    #   path to config file
    #   --<broker|clientproxy|notifybroker|worker|client>=<all|name1[,name2]*>*
    # i.e. main.py ./test.cfg --broker=all --worker=worker1,worker2
    clusterConfigFilePath = argv[1]
    filters = {}

    for filter in argv[2:]:
        filterComponents = filter[2:].split( '=' )
        filters[ filterComponents[0] ] = \
            filterComponents[1].split( ',' )

    with open( clusterConfigFilePath, "r" ) as clusterConfigFile:
        clusterConfig = \
            json.load(
                clusterConfigFile,
                object_pairs_hook=clusterHook )
        return PyLHClusterRun( clusterConfig, filters )
