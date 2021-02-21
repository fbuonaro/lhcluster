#ifndef __LHCLUSTER_BROKER_H__
#define __LHCLUSTER_BROKER_H__

#include <lhcluster/ibroker.h>

#include <algorithm>
#include <memory.h>

namespace LHClusterNS
{
    class BrokerFactory;

    class Broker : public IBroker
    {
        friend BrokerFactory;

        public:
            // CTORS, DTORS
            ~Broker()
            {
            }

            Broker( Broker&& other )
            :   IBroker( std::move( other ) )
            ,   ibroker( std::move( other.ibroker ) )
            {
            }

            Broker& operator=( Broker other )
            {
                IBroker::operator=( other );
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( Broker& a, Broker& b )
            {
                using std::swap;

                // TODO - detect if swappable
                // swap( static_cast< IBroker& >( a ), static_cast< IBroker& >( b ) );
                swap( a.ibroker, b.ibroker );
            }

            // start polling incoming messages
            void Start()
            {
                ibroker->Start();
            }

            // stop polling incoming messages
            void Stop()
            {
                ibroker->Stop();
            }

            // start polling in the current thread,
            // taking control of the thread until canceled
            void StartAsPrimary()
            {
                ibroker->StartAsPrimary();
            }

        private:
            // CTORS
            Broker( std::unique_ptr< IBroker > _ibroker )
            :   IBroker()
            ,   ibroker( move( _ibroker ) )
            {
            }

            // MEMBERS
            std::unique_ptr< IBroker > ibroker;
    };
}

#endif
