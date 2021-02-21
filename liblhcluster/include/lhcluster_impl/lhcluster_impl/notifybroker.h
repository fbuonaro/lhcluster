#ifndef __LHCLUSTER_IMPL_NOTIFYBROKER_H__
#define __LHCLUSTER_IMPL_NOTIFYBROKER_H__

#include <lhcluster/inotifybroker.h>
#include <lhcluster/endpoint.h>
#include <lhcluster_impl/zmqaddin.h>

#include <thread>

#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>

#define proxy_frontend_endpoint( proxy, endpoint_path ) \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... creating frontend ..." ); \
    zstr_sendx( proxy, "FRONTEND", "XPUB", endpoint_path, NULL ); \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... waiting for frontend to be created ..." ); \
    zsock_wait( proxy )

#define proxy_backend_endpoint( proxy, endpoint_path ) \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... creating backend ..." ); \
    zstr_sendx( proxy, "BACKEND", "XSUB", endpoint_path, NULL ); \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... waiting for backend to be created ..." ); \
    zsock_wait( proxy )

#define proxy_resume( proxy ) \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... resuming ..." ); \
    zstr_sendx( proxy, "RESUME", NULL ); \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... waiting to resume ..." ); \
    zsock_wait( proxy )

#define proxy_pause( proxy ) \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... pausing ..." ); \
    zstr_sendx( proxy, "PAUSE", NULL ); \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... waiting to pause ..." ); \
    zsock_wait( proxy )

#define proxy_verbose( proxy ) \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... turning on verbose ..." ); \
    zstr_sendx( proxy, "VERBOSE", NULL ); \
    clusterLogWithSeverity( LHClusterNS::SeverityLevel::Trace, "NotifyBroker: ... waiting for verbose ..." ); \
    zsock_wait( proxy )

namespace LHClusterNS
{
namespace Impl
{
    class NotifyBroker : public INotifyBroker
    {
        public:
            // DATA MEMBERS
            Endpoint    self_frontend_endpoint;
            Endpoint    self_backend_endpoint;
            zactor_t*   proxy;

            // CTORS, DTORS
            NotifyBroker( const NotifyBroker& other ) = delete;
            NotifyBroker( NotifyBroker&& other ) = delete;
            NotifyBroker& operator=( const NotifyBroker& other ) = delete;
            NotifyBroker& operator=( NotifyBroker&& other ) = delete;
            NotifyBroker() = delete;

            NotifyBroker(   const Endpoint& _self_frontend_endpoint,
                            const Endpoint& _self_backend_endpoint );
            ~NotifyBroker();

            // METHODS
            void Start();
            void Stop();
    };
}
}
#endif
