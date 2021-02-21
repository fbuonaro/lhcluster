#ifndef __LHCLUSTER_WORKERFACTORY_H__
#define __LHCLUSTER_WORKERFACTORY_H__

#include <lhcluster/irequesthandler.h>
#include <lhcluster/iworkerfactory.h>
#include <lhcluster/workerparameters.h>

#include <lhcluster/worker.h>

#include <algorithm>
#include <memory>

namespace LHClusterNS
{
    class WorkerFactory : public IWorkerFactory
    {
        public:
            // CTORS, DTORS
            WorkerFactory()
            :   IWorkerFactory()
            ,   iworkerfactory( IWorkerFactory::GetDefaultFactory() )
            {
            }

            WorkerFactory( WorkerFactory&& other )
            :   IWorkerFactory( std::move( other ) )
            ,   iworkerfactory( std::move( other.iworkerfactory ) )
            {
            }

            WorkerFactory& operator=( WorkerFactory other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( WorkerFactory& a, WorkerFactory& b )
            {
                using std::swap;

                swap( a.iworkerfactory, b.iworkerfactory );
            }

            Worker CreateConcrete(
                const WorkerParameters& p, std::unique_ptr< IRequestHandler > rh )
            {
                return Worker( iworkerfactory->Create( p, std::move( rh ) ) );
            }

            std::unique_ptr< IWorker > Create(
                const WorkerParameters& p, std::unique_ptr< IRequestHandler > rh )
            {
                return std::unique_ptr< IWorker >(
                    new Worker( iworkerfactory->Create( p, std::move( rh ) ) ) );
            }

        private:
            // MEMBERS
            std::unique_ptr< IWorkerFactory > iworkerfactory;
    };
}

#endif
