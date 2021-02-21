#include <lhcluster_impl/workerinfolookup.h>

namespace LHClusterNS
{
namespace Impl
{
    WorkerInfo* WorkerInfoLookup::WorkerInfoForIterator(
        WorkerInfoLookup::WILMapType::iterator it )
    {
        WorkerInfo* ret = nullptr;

        if( it != this->workerInfoMap.end() )
        {
            std::unique_ptr<WorkerInfo>& lp_workerInfo = (*it).second;
            ret = lp_workerInfo.get();
        }

        return ret;
    }


    WorkerInfo* WorkerInfoLookup::LookupWorkerInfo( const ZMQFrame* identity )
    {
        WILMapType::iterator keyValueWorkerInfo =
            this->workerInfoMap.find( identity );
        WorkerInfo* workerInfo = nullptr;

        if( keyValueWorkerInfo != workerInfoMap.end() )
            workerInfo = keyValueWorkerInfo->second.get();

        return workerInfo;
    }

    void WorkerInfoLookup::InsertWorkerInfo(
        std::unique_ptr< WorkerInfo > workerInfo  )
    {
        workerInfoMap.insert(
            std::move(
                WILMapType::value_type(
                    workerInfo->GetIdentity(),
                    std::move( workerInfo ) ) ) );
    }

    void WorkerInfoLookup::RemoveWorkerInfo(
        WorkerInfoLookup::WILMapType::iterator it )
    {
        workerInfoMap.erase( it );
    }

    void WorkerInfoLookup::RemoveWorkerInfo(
        const ZMQFrame* identity )
    {
        workerInfoMap.erase( identity );
    }

    WorkerInfoLookup::WILMapType& WorkerInfoLookup::UnderlyingMap()
    {
        return workerInfoMap;
    }

    void WorkerInfoLookup::Clear()
    {
        workerInfoMap.clear();
    }
}
}
