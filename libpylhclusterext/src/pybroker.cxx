#include <pylhclusterext/pygilguard.h>

#include <lhcluster/broker.h>
#include <lhcluster/brokerfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace LHClusterNS
{
    namespace Python
    {
        namespace
        {
            // Here is the story
            // SO
            // Broker is a move-only class
            // boost::python at this time does not support move only
            // THEREFORE
            // The BrokerFactory which returns by value cannot be used because
            // the resulting rvalue from CreateConcrete
            // cannot be used to construct a new instance of a broker
            // BUT
            // I would still like to use the Factory classes
            // because I spent a lot of time on them and I am too stubborn to quit
            // SO
            // I have created here a free floating function which can return a pointer
            // to a Broker (allowed) and accepts the broker factory and its parameters
            // This free floating function can then be used as the constructor
            // of the python wrapper Broker class which will be held in a shared_ptr
            // bypassing the noncopyable requirement and with no changes to the wrapper code
            // BUT WAIT THERE IS MORE
            // I am also providing a custom deleter because I want to unlock the
            // GIL during the destruction of these actor classes
            // The reason I am doing this is because the Stop action can potentially
            // cause a deadlock when it tries to join with another thread which is waiting for
            // the GIL to be released (right now, this can only happen with a Worker which
            // a request processor implemented in python because the Worker stop will try to join
            // with the request processor thread but the request processor thread might be trying to
            // lock GIL in order to call back into the python handler implementation)
            boost::shared_ptr< Broker > proxyCreateViaFactory(
                BrokerFactory& brokerFactory,
                const BrokerParameters& p )
            {
                // ughh, create a CreatePtr variation ??
                return boost::shared_ptr< Broker >(
                    new Broker( brokerFactory.CreateConcrete( p ) ),
                    DeleteWithNoGIL< Broker > );
            }

        }

        void ExportPythonBroker()
        {
            using namespace boost::python;

            class_<
                Broker,
                boost::noncopyable, boost::shared_ptr< Broker > >(
                    "Broker", no_init )
                .def( "__init__", make_constructor( &proxyCreateViaFactory ) )
                .def( "Start", with< noGIL >( &Broker::Start ) )
                .def( "StartAsPrimary", with< noGIL >( &Broker::StartAsPrimary ) )
                .def( "Stop", with< noGIL >( &Broker::Stop ) );
        }
    }
}
