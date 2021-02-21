#!/usr/bin/python3.6

from pylhcluster import plhc_ext

def CreateBrokerParametersBuilder(
    instanceNum=0,
    processNum=0,
    threadNum=0,
    frontendEndpointType=plhc_ext.EndpointType.InterThread,
    frontendEndpointBaseAddress="quickstart.broker_frontend",
    frontendEndpointPort=0,
    backendEndpointType=plhc_ext.EndpointType.InterThread,
    backendEndpointBaseAddress="quickstart.broker_backend",
    backendEndpointPort=0,
    controlEndpointBaseAddress="quickstart.broker_control",
    selfHeartbeatDelayS=10,
    minWorkerHeartbeatDelayS=15,
    workerPulseDelayS=20 ):
    bpb = plhc_ext.BrokerParametersBuilder()
    bpb.SetFrontendEndpointType( frontendEndpointType ) \
       .SetFrontendEndpointBaseAddress( frontendEndpointBaseAddress ) \
       .SetBackendEndpointType( backendEndpointType ) \
       .SetBackendEndpointBaseAddress( backendEndpointBaseAddress ) \
       .SetControlEndpointBaseAddress( controlEndpointBaseAddress ) \
       .SetSelfHeartbeatDelay( plhc_ext.Delay( selfHeartbeatDelayS ) ) \
       .SetMinimumWorkerHeartbeatDelay( plhc_ext.Delay( minWorkerHeartbeatDelayS ) ) \
       .SetWorkerPulseDelay( plhc_ext.Delay( workerPulseDelayS ) ) \
       .SetInstance( instanceNum ) \
       .SetProcess( processNum ) \
       .SetThread( threadNum )
    if frontendEndpointPort:
        bpb.SetFrontendEndpointPort( frontendEndpointPort )
    if backendEndpointPort:
        bpb.SetBackendEndpointPort( backendEndpointPort )

    return bpb

def CreateClientProxyParametersBuilder(
    brokerFrontendEndpoint,
    instanceNum=0,
    processNum=0,
    threadNum=0,
    selfBrokerMessagingEndpointType=plhc_ext.EndpointType.InterThread,
    selfBrokerMessagingEndpointBaseAddress="quickstart.clientproxy_broker",
    selfClientMessagingEndpointBaseAddress="quickstart.clientproxy_client",
    selfBrokerMessagingEndpointPort=0,
    selfClientMessagingEndpointType=plhc_ext.EndpointType.InterThread,
    selfClientMessagingEndpointPort=0,
    controlEndpointBaseAddress="quickstart.clientproxy_control",
    initialBrokerHeartbeatDelayS=10,
    selfHeartbeatDelayS=10,
    brokerPulseDelayS=15 ):
    cppb = plhc_ext.ClientProxyParametersBuilder()
    cppb.SetBrokerEndpoint( brokerFrontendEndpoint ) \
        .SetInstance( instanceNum ) \
        .SetProcess( processNum ) \
        .SetThread( threadNum ) \
        .SetSelfBrokerMessagingEndpointType( selfBrokerMessagingEndpointType ) \
        .SetSelfBrokerMessagingEndpointBaseAddress( selfBrokerMessagingEndpointBaseAddress ) \
        .SetSelfClientMessagingEndpointType( selfClientMessagingEndpointType ) \
        .SetSelfClientMessagingEndpointBaseAddress( selfClientMessagingEndpointBaseAddress ) \
        .SetControlEndpointBaseAddress( controlEndpointBaseAddress ) \
        .SetInitialBrokerHeartbeatDelay( plhc_ext.Delay( initialBrokerHeartbeatDelayS ) ) \
        .SetSelfHeartbeatDelay( plhc_ext.Delay( selfHeartbeatDelayS ) ) \
        .SetBrokerPulseDelay( plhc_ext.Delay( brokerPulseDelayS ) )
    if selfBrokerMessagingEndpointPort:
        cppb.SetSelfBrokerMessagingEndpointPort( selfBrokerMessagingEndpointPort )
    if selfClientMessagingEndpointPort:
        cppb.SetSelfClientMessagingEndpointPort( selfClientMessagingEndpointPort )

    return cppb

def CreateClientParametersBuilder(
    clientProxyClientMessagingEndpoint,
    instanceNum=0,
    processNum=0,
    threadNum=0,
    clientEndpointType=plhc_ext.EndpointType.InterThread,
    clientEndpointBaseAddress="quickstart.client_self",
    clientEndpointPort=0 ):
    cpb = plhc_ext.ClientParametersBuilder()
    cpb.SetClientEndpointType( clientEndpointType ) \
       .SetInstance( instanceNum ) \
       .SetProcess( processNum ) \
       .SetThread( threadNum ) \
       .SetClientEndpointBaseAddress( clientEndpointBaseAddress ) \
       .SetClientProxyEndpoint( clientProxyClientMessagingEndpoint )
    if clientEndpointPort:
        cpb.SetClientEndpointPort( clientEndpointPort )

    return cpb

def CreateWorkerParametersBuilder(
    brokerBackendEndpoint,
    instanceNum=0,
    processNum=0,
    threadNum=0,
    selfMessagingEndpointType=plhc_ext.EndpointType.InterThread,
    selfMessagingEndpointBaseAddress="quickstart.worker_self",
    selfMessagingEndpointPort=0,
    selfControllerEndpointBaseAddress="quickstart.worker_control",
    selfInternalEndpointType=plhc_ext.EndpointType.InterThread,
    selfInternalEndpointBaseAddress="quickstart.worker_internal",
    selfInternalEndpointPort=0,
    requestHandlerControllerEndpointBaseAddress="quickstart.worker_rh_control",
    requestHandlerInternalEndpointType=plhc_ext.EndpointType.InterThread,
    requestHandlerInternalEndpointBaseAddress="quickstart.worker_rh_internal",
    requestHandlerInternalEndpointPort=0,
    selfHeartbeatDelayS=15,
    initialBrokerHeartbeatDelayS=15,
    brokerPulseDelayS=15 ):
    wpb = plhc_ext.WorkerParametersBuilder()
    wpb.SetBrokerMessagingEndpoint( brokerBackendEndpoint ) \
       .SetInstance( instanceNum ) \
       .SetProcess( processNum ) \
       .SetThread( threadNum ) \
       .SetSelfMessagingEndpointType( selfMessagingEndpointType ) \
       .SetSelfMessagingEndpointBaseAddress( selfMessagingEndpointBaseAddress ) \
       .SetSelfControllerEndpointBaseAddress( selfControllerEndpointBaseAddress ) \
       .SetSelfInternalEndpointType( selfInternalEndpointType ) \
       .SetSelfInternalEndpointBaseAddress( selfInternalEndpointBaseAddress ) \
       .SetRequestHandlerControllerEndpointBaseAddress(
            requestHandlerControllerEndpointBaseAddress ) \
       .SetRequestHandlerInternalEndpointType( requestHandlerInternalEndpointType ) \
       .SetRequestHandlerInternalEndpointBaseAddress(
            requestHandlerInternalEndpointBaseAddress ) \
       .SetSelfHeartbeatDelay( plhc_ext.Delay( selfHeartbeatDelayS ) ) \
       .SetInitialBrokerHeartbeatDelay( plhc_ext.Delay( initialBrokerHeartbeatDelayS ) ) \
       .SetBrokerPulseDelay( plhc_ext.Delay( brokerPulseDelayS ) )
    if selfMessagingEndpointPort:
        wpb.SetSelfMessagingEndpointPort( selfMessagingEndpointPort )
    if selfInternalEndpointPort:
        wpb.SetSelfInternalEndpointPort( selfInternalEndpointPort )
    if requestHandlerInternalEndpointPort:
        wpb.SetRequestHandlerInternalEndpointPort( requestHandlerInternalEndpointPort )

    return wpb

def CreateNotifySubscriberParametersBuilder(
    publisherEndpoint,
    instanceNum=0,
    processNum=0,
    threadNum=0,
    selfControllerEndpointBaseAddress="quickstart.notifysubscriber_control" ):
    nspb = plhc_ext.NotifySubscriberParametersBuilder()
    nspb.SetPublisherEndpoint( publisherEndpoint ) \
        .SetSelfControllerEndpointBaseAddress( selfControllerEndpointBaseAddress ) \
        .SetInstance( instanceNum ) \
        .SetProcess( processNum ) \
        .SetThread( threadNum )

    return nspb

def CreateNotifyPublisherParametersBuilder(
    proxyEndpoint=None,
    instanceNum=0,
    processNum=0,
    threadNum=0,
    selfEndpointType=plhc_ext.EndpointType.InterThread,
    selfEndpointBaseAddress="quickstart.notifypublisher_self",
    selfEndpointPort=0 ):
    nppb = plhc_ext.NotifyPublisherParametersBuilder()
    nppb.SetInstance( instanceNum ) \
        .SetProcess( processNum ) \
        .SetThread( threadNum )
    if proxyEndpoint:
        nppb.SetProxyEndpoint( proxyEndpoint )
    else:
        nppb.SetSelfEndpointType( selfEndpointType ) \
            .SetSelfEndpointBaseAddress( selfEndpointBaseAddress )
        if selfEndpointPort:
            nppb.SetSelfEndpointPort( selfEndpointPort )

    return nppb 

def CreateNotifyBrokerParametersBuilder(
    instanceNum=0,
    processNum=0,
    threadNum=0,
    frontendEndpointType=plhc_ext.EndpointType.InterThread,
    frontendEndpointBaseAddress="quickstart.notifybroker_frontend",
    frontendEndpointPort=0,
    backendEndpointType=plhc_ext.EndpointType.InterThread,
    backendEndpointBaseAddress="quickstart.notifybroker_backend",
    backendEndpointPort=0 ):
    nbpb = plhc_ext.NotifyBrokerParametersBuilder()
    nbpb.SetFrontendEndpointType( frontendEndpointType ) \
        .SetFrontendEndpointBaseAddress( frontendEndpointBaseAddress ) \
        .SetBackendEndpointType( backendEndpointType ) \
        .SetBackendEndpointBaseAddress( backendEndpointBaseAddress ) \
        .SetInstance( instanceNum ) \
        .SetProcess( processNum ) \
        .SetThread( threadNum )
    if frontendEndpointPort:
        nbpb.SetFrontendEndpointPort( frontendEndpointPort )
    if backendEndpointPort:
        nbpb.SetBackendEndpointPort( backendEndpointPort )

    return nbpb 

def CreateBrokerFromParameters( brokerParameters ):
    brokerFactory = plhc_ext.BrokerFactory()

    return plhc_ext.Broker( brokerFactory, brokerParameters )

def CreateClientProxyFromParameters( clientProxyParameters ):
    clientProxyFactory = plhc_ext.ClientProxyFactory()

    return plhc_ext.ClientProxy( clientProxyFactory, clientProxyParameters )

def CreateClientFromParameters( clientParameters ):
    clientFactory = plhc_ext.ClientFactory()

    return plhc_ext.Client( clientFactory, clientParameters )

def CreateWorkerFromParameters( workerParameters, requestHandler ):
    workerFactory = plhc_ext.WorkerFactory()

    return plhc_ext.Worker( workerFactory, workerParameters, requestHandler )

def CreateNotifyBroker( notifyBrokerParameters ):
    notifyBrokerFactory = plhc_ext.NotifyBrokerFactory()

    return plhc_ext.NotifyBroker( notifyBrokerFactory, notifyBrokerParameters )

def CreateNotifyPublisher( notifyPublisherParameters ):
    notifyPublisherFactory = plhc_ext.NotifyPublisherFactory()

    return plhc_ext.NotifyPublisher( notifyPublisherFactory, notifyPublisherParameters )

def CreateNotifySubscriber( notifySubscriberParameters, notifyTypes, notifyHandler ):
    notifySubscriberFactory = plhc_ext.NotifySubscriberFactory()

    return plhc_ext.NotifySubscriber(
        notifySubscriberFactory, notifySubscriberParameters, notifyTypes, notifyHandler )

def CreateEndpointFromDict( endpointDict ):
    endpointType = endpointDict[ "endpointType" ]
    if( endpointType == plhc_ext.EndpointType.TCP ):
        return plhc_ext.Endpoint( \
            endpointType,
            "{}:{}".format( endpointDict[ "endpointBaseAddress" ],
                            endpointDict[ "endpointPort" ] ) )
    else:
        return plhc_ext.Endpoint( \
            endpointType,
            endpointDict[ "endpointBaseAddress" ] )
