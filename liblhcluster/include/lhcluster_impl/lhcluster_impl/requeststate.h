#ifndef __LHCLUSTER_IMPL_REQUESTSTATE_H__
#define __LHCLUSTER_IMPL_REQUESTSTATE_H__

#include <lhcluster/cluster.h>
#include <lhcluster_impl/zmqaddin.h>

#include <list>

namespace LHClusterNS
{
namespace Impl
{
    class WorkerInfo;

    enum class RequestStateFlag : std::uint8_t
    {
        None,
        Pending,
        InProgress
    };

    class RequestState
    {
        public:
            // CTORS, DTORS
            RequestState() = delete;
            RequestState( const RequestState& ) = delete;
            RequestState( RequestState&& ) = delete;
            RequestState& operator=( const RequestState& ) = delete;
            RequestState& operator=( RequestState&& ) = delete;

            RequestState(
                RequestId _requestId,
                const LHCVersionFlags& vfs,
                ZMQMessage** lpMsg );
            RequestState(
                RequestId _requestId,
                const LHCVersionFlags& vfs,
                WorkerInfo* wi );
            ~RequestState();

            // METHODS
            bool IsPending() const
            {
                return flag == RequestStateFlag::Pending;
            }

            bool IsInProgress() const
            {
                return flag == RequestStateFlag::InProgress;
            }

            ZMQMessage** GetMessage()
            {
                // assert pending
                return &msg;
            }

            RequestId GetRequestId() const
            {
                return requestId;
            }

            const LHCVersionFlags& GetVersionAndFlags() const
            {
                return vfs;
            }

            short GetTryCount() const
            {
                return tryCount;
            }

            const WorkerInfo* GetAssignedWorker() const
            {
                // assert in progress
                return state.inprogress.assignedWorker;
            }

            WorkerInfo* GetAssignedWorker()
            {
                // assert in progress
                return state.inprogress.assignedWorker;
            }

            std::list< RequestState* >::iterator GetQueueCursor()
            {
                // assert pending
                return state.pending.listIt;
            }

            void Assign( WorkerInfo* wi );

            void UpdateRequest( ZMQMessage** lpMsg );
            void UpdateRequest( const LHCVersionFlags& _vfs );
            void UpdateRequest( ZMQMessage** lpMsg, const LHCVersionFlags& _vfs );
            void SetRequest( ZMQMessage** lpMsg, const LHCVersionFlags& _vfs );

            void SetQueueCursor( std::list< RequestState* >::iterator _listIt );

        private:
            // DATA MEMBERS
            RequestId requestId;
            RequestStateFlag flag;
            LHCVersionFlags vfs;
            ZMQMessage* msg;
            short tryCount;
            union
            {
                struct
                {
                    std::list< RequestState* >::iterator listIt;
                } pending;

                struct
                {
                    WorkerInfo* assignedWorker;
                } inprogress;
            } state;
    };
}
}

#endif
