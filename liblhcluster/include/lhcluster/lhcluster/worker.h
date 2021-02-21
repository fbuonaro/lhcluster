#ifndef __LHCLUSTER_WORKER_H__
#define __LHCLUSTER_WORKER_H__

#include <lhcluster/iworker.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class WorkerFactory;

    class Worker : public IWorker
    {
        friend WorkerFactory;

        public:
            // CTORS, DTORS
            ~Worker()
            {
            }

            Worker( Worker&& other )
            :   IWorker( std::move( other ) )
            ,   iworker( std::move( other.iworker ) )
            {
            }

            Worker& operator=( Worker other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( Worker& a, Worker& b )
            {
                using std::swap;

                swap( a.iworker, b.iworker );
            }

            // start polling incoming messages in a secondary thread
            void Start()
            {
                iworker->Start();
            }

            // start polling incoming messages in this thread
            void StartAsPrimary()
            {
                iworker->StartAsPrimary();
            }

            // stop polling incoming messages
            void Stop()
            {
                iworker->Stop();
            }

        private:
            // CTORS
            Worker( std::unique_ptr< IWorker > _iworker )
            :   IWorker()
            ,   iworker( move( _iworker ) )
            {
            }

            // MEMBERS
            std::unique_ptr< IWorker > iworker;
    };
}
#endif
