#ifndef __LHCLUSTER_IMPL_WORKERINFOLOOKUP_H__
#define __LHCLUSTER_IMPL_WORKERINFOLOOKUP_H__

#include <lhcluster_impl/hash_traits.h>
#include <lhcluster_impl/workerinfo.h>

#include <functional>
#include <unordered_map>
#include <memory>

namespace LHClusterNS
{
namespace Impl
{
    class WorkerInfoLookup
    {
        public:
            WorkerInfoLookup( const WorkerInfoLookup& ) = delete;
            WorkerInfoLookup( WorkerInfoLookup&& ) = delete;
            WorkerInfoLookup& operator=( const WorkerInfoLookup& ) = delete;
            WorkerInfoLookup& operator=( WorkerInfoLookup&& ) = delete;

            WorkerInfoLookup() = default;

            // hash from  ZMQFrame (identity) to a WorkerInfo
            typedef std::unordered_map<
                const ZMQFrame*,
                std::unique_ptr< WorkerInfo >,
                std::hash< const ZMQFrame* > > WILMapType;

            // METHODS
            WorkerInfo* WorkerInfoForIterator( WILMapType::iterator );
            WorkerInfo* LookupWorkerInfo( const ZMQFrame* identity );
            // takes ownership of passed in workerInfo
            void InsertWorkerInfo( std::unique_ptr< WorkerInfo > workerInfo );
            void RemoveWorkerInfo( const ZMQFrame* identity );
            void RemoveWorkerInfo( WILMapType::iterator it );
            //used to allow owner of map to iterate over each item
            WILMapType& UnderlyingMap();
            void Clear();

        private:
            WILMapType workerInfoMap;

    };
}
}

#endif
