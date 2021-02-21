#include <lhcluster_impl/debug.h>
#include <lhcluster_impl/notifybroker.h>

#include <stdexcept>

namespace LHClusterNS
{
namespace Impl
{
    NotifyBroker::NotifyBroker( const Endpoint& _self_frontend_endpoint,
                                const Endpoint& _self_backend_endpoint )
    :   INotifyBroker()
    ,   self_frontend_endpoint( _self_frontend_endpoint )
    ,   self_backend_endpoint( _self_backend_endpoint )
    ,   proxy( nullptr )
    {
    }

    void NotifyBroker::Start()
    {
        clusterLogSetAction( "NotifyBroker.start" );
        clusterLog( "NotifyBroker.start: begin" );

        clusterLog( "NotifyBroker.start: starting with parameters\n" <<
                    "    self_frontend_endpoint[" << self_frontend_endpoint << "]\n" <<
                    "    self_backend_endpoint[" << self_backend_endpoint << "]" );
        if( !( proxy ) )
        {
            proxy = zactor_new( zproxy, NULL );
            if( !( proxy ) )
                throw std::runtime_error( "Could not create NotifyBroker proxy" );
#ifdef DEBUG
            proxy_verbose( proxy );
#endif
            proxy_frontend_endpoint( proxy, self_frontend_endpoint.path() );
            proxy_backend_endpoint( proxy, self_backend_endpoint.path() );
        }
#ifdef DEBUG
        else
        {
            clusterLog( "NotifyBroker.start: already started" );
        }
#endif

        clusterLog( "NotifyBroker.start: end" );
    }

    void NotifyBroker::Stop()
    {
        if( proxy )
        {
            zactor_destroy( &proxy );
        }
    }

    NotifyBroker::~NotifyBroker()
    {
        Stop();
    }
}
}
