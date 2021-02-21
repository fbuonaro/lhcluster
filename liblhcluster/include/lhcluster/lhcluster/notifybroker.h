#ifndef __LHCLUSTER_NOTIFYBROKER_H__
#define __LHCLUSTER_NOTIFYBROKER_H__

#include <lhcluster/inotifybroker.h>

#include <algorithm>
#include <memory.h>

namespace LHClusterNS
{
    class NotifyBrokerFactory;

    class NotifyBroker : public INotifyBroker
    {
        friend NotifyBrokerFactory;

        public:
            // CTORS, DTORS
            ~NotifyBroker()
            {
            }

            NotifyBroker( NotifyBroker&& other )
            :   INotifyBroker( std::move( other ) )
            ,   inotifybroker( std::move( other.inotifybroker ) )
            {
            }

            NotifyBroker& operator=( NotifyBroker other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( NotifyBroker& a, NotifyBroker& b )
            {
                using std::swap;

                swap( a.inotifybroker, b.inotifybroker );
            }

            void Start()
            {
                inotifybroker->Start();
            }

            void Stop()
            {
                inotifybroker->Stop();
            }

        private:
            // CTORS
            NotifyBroker( std::unique_ptr< INotifyBroker > _inotifybroker )
            :   INotifyBroker()
            ,   inotifybroker( move( _inotifybroker ) )
            {
            }

            // MEMBERS
            std::unique_ptr< INotifyBroker > inotifybroker;
    };
}

#endif
