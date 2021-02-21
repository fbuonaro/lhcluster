#include <lhcluster/brokerparameters.h>
#include <lhcluster/brokerparametersbuilder.h>
#include <lhcluster/clientparameters.h>
#include <lhcluster/clientparametersbuilder.h>
#include <lhcluster/clientproxyparameters.h>
#include <lhcluster/clientproxyparametersbuilder.h>
#include <lhcluster/notifybrokerparameters.h>
#include <lhcluster/notifybrokerparametersbuilder.h>
#include <lhcluster/notifypublisherparameters.h>
#include <lhcluster/notifypublisherparametersbuilder.h>
#include <lhcluster/notifysubscriberparameters.h>
#include <lhcluster/notifysubscriberparametersbuilder.h>
#include <lhcluster/workerparameters.h>
#include <lhcluster/workerparametersbuilder.h>

#include <gtest/gtest.h>

namespace TestLHClusterNS
{
    using namespace LHClusterNS;
    using namespace std;

    class TestParameters : public ::testing::Test
    {
        protected:
            void SetUp()
            {
            }

            void TearDown()
            {
            }
    };

    TEST_F( TestParameters, TestEndpointParameters )
    {
        Endpoint tcpEndpoint( EndpointType::TCP, "hello.world.com:1234" );
        Endpoint tcpEndpoint2( EndpointType::TCP, "hello.world.com:12340" );
        Endpoint thrEndpoint( EndpointType::InterThread, "some_thread_ep" );
        Endpoint ipcEndpoint( EndpointType::InterProcess, "some_process_ep.zipc" );
        Endpoint ipcEndpoint2( EndpointType::InterProcess, "some_process_ep_1_2_3.zipc" );
        EndpointBuilder builder;
        Endpoint builtEndpoint;

        // By components, TCP
        builder
        .SetEndpointType( EndpointType::TCP )
        .SetBaseAddress( "hello.world.com" )
        .SetPort( 1234 );

        ASSERT_EQ( EndpointType::TCP, builder.GetEndpointType() );
        ASSERT_EQ( "hello.world.com", builder.GetBaseAddress() );
        ASSERT_EQ( 1234, builder.GetPort() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( tcpEndpoint, builtEndpoint );

        builder.SetPort( 12340 );
        ASSERT_EQ( 12340, builder.GetPort() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( tcpEndpoint2, builtEndpoint );

        // By Endpoint
        builder
        .SetEndpoint( thrEndpoint );
        ASSERT_EQ( thrEndpoint, builder.GetEndpoint() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( thrEndpoint, builtEndpoint );

        // By components, IPC
        builder
        .SetEndpointType( EndpointType::InterProcess )
        .SetBaseAddress( "some_process_ep" );

        builtEndpoint = builder.Build();
        ASSERT_EQ( ipcEndpoint, builtEndpoint );

        builder.SetInstance( 1 );
        ASSERT_EQ( 1, builder.GetInstance() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( ipcEndpoint, builtEndpoint ); // no change because thread, process still 0

        builder
        .SetProcess( 2 )
        .SetThread( 3 );
        ASSERT_EQ( 2, builder.GetProcess() );
        ASSERT_EQ( 3, builder.GetThread() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( ipcEndpoint2, builtEndpoint );

        builder
        .SetProcess( 0 );

        ASSERT_EQ( 0, builder.GetProcess() );

        builtEndpoint = builder.Build();
        ASSERT_EQ( ipcEndpoint, builtEndpoint ); // back to original format b/c one of inst, proc,
                                                 // thread is 0
    }

    TEST_F( TestParameters, TestBrokerParameters )
    {
        Endpoint frontendMessaging( EndpointType::TCP, "hello.world:22" );
        Endpoint backendMessaging( EndpointType::InterProcess, "hello_1_1_1.zipc" );
        Endpoint control( EndpointType::InterThread, "control_1_1_1.zitc" );
        Delay selfHb( 1 );
        Delay minWorkerHb( 100 );
        Delay pulse( 33 );

        BrokerParameters p( frontendMessaging, backendMessaging, control, selfHb,
                            minWorkerHb, pulse );

        ASSERT_EQ( frontendMessaging, p.GetFrontendMessagingEndpoint() );
        ASSERT_EQ( backendMessaging, p.GetBackendMessagingEndpoint() );
        ASSERT_EQ( control, p.GetControlEndpoint() );
        ASSERT_EQ( selfHb, p.GetSelfHeartbeatDelay() );
        ASSERT_EQ( minWorkerHb, p.GetMinimumWorkerHeartbeatDelay() );
        ASSERT_EQ( pulse, p.GetWorkerPulseDelay() );

        BrokerParametersBuilder pBuilder;

        pBuilder
        .SetInstance( 1 )
        .SetProcess( 1 )
        .SetThread( 1 )
        .SetFrontendEndpointType( EndpointType::TCP )
        .SetFrontendEndpointBaseAddress( "hello.world" )
        .SetFrontendEndpointPort( 22 )
        .SetBackendEndpointType( EndpointType::InterProcess )
        .SetBackendEndpointBaseAddress( "hello" )
        .SetControlEndpointBaseAddress( "control" )
        .SetSelfHeartbeatDelay( selfHb )
        .SetMinimumWorkerHeartbeatDelay( minWorkerHb )
        .SetWorkerPulseDelay( pulse );

        BrokerParameters built( pBuilder.Build() );

        ASSERT_EQ( p.GetFrontendMessagingEndpoint(), built.GetFrontendMessagingEndpoint() );
        ASSERT_EQ( p.GetBackendMessagingEndpoint(), built.GetBackendMessagingEndpoint() );
        ASSERT_EQ( p.GetControlEndpoint(), built.GetControlEndpoint() );
        ASSERT_EQ( p.GetSelfHeartbeatDelay(), built.GetSelfHeartbeatDelay() );
        ASSERT_EQ( p.GetMinimumWorkerHeartbeatDelay(), built.GetMinimumWorkerHeartbeatDelay() );
        ASSERT_EQ( p.GetWorkerPulseDelay(), built.GetWorkerPulseDelay() );

        ASSERT_EQ( p, built );
    }

    TEST_F( TestParameters, TestClientParameters )
    {
        Endpoint clientMessaging( EndpointType::TCP, "hello.world:22" );
        Endpoint clientProxyMessaging( EndpointType::InterProcess, "hello_1_1_1.zipc" );

        ClientParameters p( clientMessaging, clientProxyMessaging );

        ASSERT_EQ( clientMessaging, p.GetClientMessagingEndpoint() );
        ASSERT_EQ( clientProxyMessaging, p.GetClientProxyMessagingEndpoint() );

        ClientParametersBuilder pBuilder;

        pBuilder
        .SetClientEndpointType( EndpointType::TCP )
        .SetClientEndpointBaseAddress( "hello.world" )
        .SetClientEndpointPort( 22 )
        .SetClientProxyEndpoint( clientProxyMessaging );

        ClientParameters built( pBuilder.Build() );

        ASSERT_EQ( clientMessaging, built.GetClientMessagingEndpoint() );
        ASSERT_EQ( clientProxyMessaging, built.GetClientProxyMessagingEndpoint() );

        ASSERT_EQ( p, built );
    }

    TEST_F( TestParameters, TestClientProxyParameters )
    {
        Endpoint selfBrokerMessaging( EndpointType::TCP, "hello.world:22" );
        Endpoint brokerMessaging( EndpointType::InterProcess, "hello_1_1_1.zipc" );
        Endpoint control( EndpointType::InterThread, "control_1_1_1.zitc" );
        Endpoint selfClientMessaging( EndpointType::InterThread, "client_1_1_1.zitc" );
        Delay selfHb( 1 );
        Delay initBroker( 100 );
        Delay brokerPulse( 33 );

        ClientProxyParameters p( selfBrokerMessaging, brokerMessaging, control,
                                 selfClientMessaging,
                                 selfHb, initBroker, brokerPulse );

        ASSERT_EQ( selfBrokerMessaging, p.GetSelfBrokerMessagingEndpoint() );
        ASSERT_EQ( brokerMessaging, p.GetBrokerMessagingEndpoint() );
        ASSERT_EQ( control, p.GetSelfControllerEndpoint() );
        ASSERT_EQ( selfClientMessaging, p.GetSelfClientMessagingEndpoint() );
        ASSERT_EQ( selfHb, p.GetSelfHeartbeatDelay() );
        ASSERT_EQ( initBroker, p.GetInitialBrokerHeartbeatDelay() );
        ASSERT_EQ( brokerPulse, p.GetBrokerPulseDelay() );

        ClientProxyParametersBuilder pBuilder;

        pBuilder
        .SetInstance( 1 )
        .SetProcess( 1 )
        .SetThread( 1 )
        .SetSelfBrokerMessagingEndpointType( EndpointType::TCP )
        .SetSelfBrokerMessagingEndpointBaseAddress( "hello.world" )
        .SetSelfBrokerMessagingEndpointPort( 22 )
        .SetBrokerEndpoint( brokerMessaging )
        .SetControlEndpointBaseAddress( "control" )
        .SetSelfClientMessagingEndpointType( EndpointType::InterThread )
        .SetSelfClientMessagingEndpointBaseAddress( "client" )
        .SetSelfHeartbeatDelay( selfHb )
        .SetInitialBrokerHeartbeatDelay( initBroker )
        .SetBrokerPulseDelay( brokerPulse );

        ClientProxyParameters built( pBuilder.Build() );

        ASSERT_EQ( selfBrokerMessaging, built.GetSelfBrokerMessagingEndpoint() );
        ASSERT_EQ( brokerMessaging, built.GetBrokerMessagingEndpoint() );
        ASSERT_EQ( control, built.GetSelfControllerEndpoint() );
        ASSERT_EQ( selfClientMessaging, built.GetSelfClientMessagingEndpoint() );
        ASSERT_EQ( selfHb, built.GetSelfHeartbeatDelay() );
        ASSERT_EQ( initBroker, built.GetInitialBrokerHeartbeatDelay() );
        ASSERT_EQ( brokerPulse, built.GetBrokerPulseDelay() );

        ASSERT_EQ( p, built );

    }

    TEST_F( TestParameters, TestNotifyBrokerParameters )
    {
        Endpoint frontendMessaging( EndpointType::TCP, "hello.world:22" );
        Endpoint backendMessaging( EndpointType::InterProcess, "hello_1_2_3.zipc" );

        NotifyBrokerParameters p( frontendMessaging, backendMessaging );

        ASSERT_EQ( frontendMessaging, p.GetFrontendMessagingEndpoint() );
        ASSERT_EQ( backendMessaging, p.GetBackendMessagingEndpoint() );

        NotifyBrokerParametersBuilder pBuilder;

        pBuilder
        .SetInstance( 1 )
        .SetProcess( 2 )
        .SetThread( 3 )
        .SetFrontendEndpointType( EndpointType::TCP )
        .SetFrontendEndpointBaseAddress( "hello.world" )
        .SetFrontendEndpointPort( 22 )
        .SetBackendEndpointType( EndpointType::InterProcess )
        .SetBackendEndpointBaseAddress( "hello" );

        NotifyBrokerParameters built( pBuilder.Build() );

        ASSERT_EQ( frontendMessaging, built.GetFrontendMessagingEndpoint() );
        ASSERT_EQ( backendMessaging, built.GetBackendMessagingEndpoint() );

        ASSERT_EQ( p, built );
    }

    TEST_F( TestParameters, TestNotifyPublisherParameters )
    {
        Endpoint proxyMessaging( EndpointType::TCP, "hello.world:22" );
        Endpoint selfEndpoint( EndpointType::InterThread, "itc_1_2_3.zitc" );
        Endpoint endpointNone( EndpointType::None, nullptr );

        NotifyPublisherParameters p( proxyMessaging, selfEndpoint );

        ASSERT_EQ( proxyMessaging, p.GetProxyEndpoint() );
        ASSERT_EQ( selfEndpoint, p.GetSelfEndpoint() );

        NotifyPublisherParametersBuilder pBuilder;

        pBuilder
        .SetProxyEndpoint( proxyMessaging )
        .SetInstance( 1 )
        .SetProcess( 2 )
        .SetThread( 3 )
        .SetSelfEndpointType( EndpointType::InterThread )
        .SetSelfEndpointBaseAddress( "itc" );

        NotifyPublisherParameters built( pBuilder.Build() );

        ASSERT_EQ( proxyMessaging, built.GetProxyEndpoint() ) << proxyMessaging.path() << " VS " << built.GetProxyEndpoint().path();
        ASSERT_EQ( selfEndpoint, built.GetSelfEndpoint() );

        ASSERT_EQ( p, built );

        pBuilder
        .SetProxyEndpoint( endpointNone );

        built = pBuilder.Build();

        ASSERT_EQ( endpointNone, built.GetProxyEndpoint() );
        ASSERT_EQ( selfEndpoint, built.GetSelfEndpoint() );

        pBuilder
        .SetProxyEndpoint( proxyMessaging )
        .SetSelfEndpointType( EndpointType::None );

        built = pBuilder.Build();

        ASSERT_EQ( proxyMessaging, built.GetProxyEndpoint() );
        ASSERT_EQ( endpointNone, built.GetSelfEndpoint() );
    }

    TEST_F( TestParameters, TestNotifySubscriberParameters )
    {
        Endpoint selfController( EndpointType::InterThread, "hello_1_1_1.zitc" );
        Endpoint publisher( EndpointType::TCP, "hello.world:22" );

        NotifySubscriberParameters p( selfController, publisher );

        ASSERT_EQ( selfController, p.GetSelfControllerEndpoint() );
        ASSERT_EQ( publisher, p.GetPublisherEndpoint() );

        NotifySubscriberParametersBuilder pBuilder;

        pBuilder
        .SetInstance( 1 )
        .SetProcess( 1 )
        .SetThread( 1 )
        .SetSelfControllerEndpointBaseAddress( "hello" )
        .SetPublisherEndpoint( publisher );

        NotifySubscriberParameters built( pBuilder.Build() );

        ASSERT_EQ( selfController, built.GetSelfControllerEndpoint() );
        ASSERT_EQ( publisher, built.GetPublisherEndpoint() );

        ASSERT_EQ( p, built );
    }

    TEST_F( TestParameters, TestWorkerParameters )
    {
        Endpoint selfMessagingEndpoint( EndpointType::TCP, "my.world.com:23" );
        Endpoint brokerMessagingEndpoint( EndpointType::TCP, "broker.world.com:23555" );
        Endpoint selfControllerEndpoint( EndpointType::InterThread, "worker_12_12_13.zitc" );
        Endpoint selfInternalEndpoint( EndpointType::InterProcess, "workeripc_12_12_13.zipc" );
        Endpoint rhInternalEndpoint( EndpointType::InterProcess, "rhipc_12_12_13.zipc" );
        Endpoint rhControllerEndpoint( EndpointType::InterThread, "rh_12_12_13.zitc" );
        Delay selfHeartbeatDelay( 123 );
        Delay initialBrokerHeartbeatDelay( 456 );
        Delay brokerPulseDelay( 789 );

        WorkerParameters p(
            selfMessagingEndpoint,
            brokerMessagingEndpoint,
            selfControllerEndpoint,
            selfInternalEndpoint,
            rhInternalEndpoint,
            rhControllerEndpoint,
            selfHeartbeatDelay,
            initialBrokerHeartbeatDelay,
            brokerPulseDelay );

        ASSERT_EQ( selfMessagingEndpoint, p.GetSelfMessagingEndpoint() );
        ASSERT_EQ( brokerMessagingEndpoint, p.GetBrokerMessagingEndpoint() );
        ASSERT_EQ( selfControllerEndpoint, p.GetSelfControllerEndpoint() );
        ASSERT_EQ( selfInternalEndpoint, p.GetSelfInternalEndpoint() );
        ASSERT_EQ( rhInternalEndpoint, p.GetRequestHandlerInternalEndpoint() );
        ASSERT_EQ( rhControllerEndpoint, p.GetRequestHandlerControllerEndpoint() );
        ASSERT_EQ( selfHeartbeatDelay, p.GetSelfHeartbeatDelay() );
        ASSERT_EQ( brokerPulseDelay, p.GetBrokerPulseDelay() );

        WorkerParametersBuilder pBuilder;

        pBuilder
        .SetInstance( 12 )
        .SetProcess( 12 )
        .SetThread( 13 )
        .SetSelfMessagingEndpointType( EndpointType::TCP )
        .SetSelfMessagingEndpointBaseAddress( "my.world.com" )
        .SetSelfMessagingEndpointPort( 23 )
        .SetBrokerMessagingEndpoint( brokerMessagingEndpoint )
        .SetSelfControllerEndpointBaseAddress( "worker" )
        .SetSelfInternalEndpointType( EndpointType::InterProcess )
        .SetSelfInternalEndpointBaseAddress( "workeripc" )
        .SetRequestHandlerInternalEndpointType( EndpointType::InterProcess )
        .SetRequestHandlerInternalEndpointBaseAddress( "rhipc" )
        .SetRequestHandlerControllerEndpointBaseAddress( "rh" )
        .SetSelfHeartbeatDelay( selfHeartbeatDelay )
        .SetInitialBrokerHeartbeatDelay( initialBrokerHeartbeatDelay )
        .SetBrokerPulseDelay( brokerPulseDelay );

        WorkerParameters built( pBuilder.Build() );

        ASSERT_EQ( selfMessagingEndpoint, built.GetSelfMessagingEndpoint() );
        ASSERT_EQ( brokerMessagingEndpoint, built.GetBrokerMessagingEndpoint() );
        ASSERT_EQ( selfControllerEndpoint, built.GetSelfControllerEndpoint() );
        ASSERT_EQ( selfInternalEndpoint, built.GetSelfInternalEndpoint() );
        ASSERT_EQ( rhInternalEndpoint, built.GetRequestHandlerInternalEndpoint() );
        ASSERT_EQ( rhControllerEndpoint, built.GetRequestHandlerControllerEndpoint() );
        ASSERT_EQ( selfHeartbeatDelay, built.GetSelfHeartbeatDelay() );
        ASSERT_EQ( brokerPulseDelay, built.GetBrokerPulseDelay() );

        ASSERT_EQ( p, built );
    }
}
