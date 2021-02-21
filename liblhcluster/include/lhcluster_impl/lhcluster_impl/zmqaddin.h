#ifndef __LHCLUSTER_IMPL_ZMQADDIN_H__
#define __LHCLUSTER_IMPL_ZMQADDIN_H__

#include <lhcluster/zmqaddin.h>

#include <czmq.h>

#include <algorithm>
#include <functional>
#include <vector>

namespace LHClusterNS
{
    typedef zsock_t ZMQSocket;
    typedef zloop_t ZMQReactor;

    typedef ZMQSocket SocketSubscriber;
    typedef ZMQSocket SocketPublisher;
    typedef ZMQSocket SocketXSubscriber;
    typedef ZMQSocket SocketXPublisher;
    typedef ZMQSocket SocketDealer;
    typedef ZMQSocket SocketRouter;
    typedef ZMQSocket SocketPair;

    typedef std::vector< ZMQFrame* > Envelope;
    typedef Envelope ClientEnvelope;

    enum SocketType
    {
        Dealer      = ZMQ_DEALER,
        Router      = ZMQ_ROUTER,
        Pair        = ZMQ_PAIR,
        Subscriber  = ZMQ_SUB,
        Publisher   = ZMQ_PUB,
        XSubscriber = ZMQ_XSUB,
        XPublisher  = ZMQ_XPUB,
    };
}

namespace std
{
    template<>
    struct equal_to< const LHClusterNS::ZMQFrame* >
    {
        bool operator()( const LHClusterNS::ZMQFrame* const &frame1,
                         const LHClusterNS::ZMQFrame* const &frame2  ) const
        {
            return zframe_eq(
                const_cast< LHClusterNS::ZMQFrame* >( frame1 ),
                const_cast< LHClusterNS::ZMQFrame* >( frame2 ) );
        }
    };

    template<>
    struct equal_to< const LHClusterNS::ClientEnvelope >
    {
        bool operator()( const LHClusterNS::ClientEnvelope &env1,
                         const LHClusterNS::ClientEnvelope &env2 ) const
        {
            return std::equal<
                LHClusterNS::ClientEnvelope::const_iterator,
                LHClusterNS::ClientEnvelope::const_iterator >( 
                    env1.begin(), env1.end(), env2.begin() );
        }
    };
}

#endif
