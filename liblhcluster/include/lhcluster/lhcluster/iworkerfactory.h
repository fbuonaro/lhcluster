#ifndef __LHCLUSTER_IWORKERFACTORY_H__
#define __LHCLUSTER_IWORKERFACTORY_H__

#include <lhcluster/irequesthandler.h>
#include <lhcluster/iworker.h>
#include <lhcluster/workerparameters.h>

#include <memory>

namespace LHClusterNS
{
    class IWorkerFactory
    {
        public:
            IWorkerFactory();
            virtual ~IWorkerFactory();

            virtual std::unique_ptr< IWorker > Create(
                const WorkerParameters& p,
                std::unique_ptr< IRequestHandler > rh ) = 0;

            static std::unique_ptr< IWorkerFactory > GetDefaultFactory();
    };
}

#endif
