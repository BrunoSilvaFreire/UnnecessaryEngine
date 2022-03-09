//
// Created by brunorbsf on 09/03/22.
//

#ifndef UNNECESSARYENGINE_ARCHETYPE_MIXIN_H
#define UNNECESSARYENGINE_ARCHETYPE_MIXIN_H

#include <unnecessary/jobs/workers/worker_pool.h>

namespace un {

    template<typename TWorker>
    class WorkerChain;

    template<typename TJobSystem>
    class JobChain;

    template<typename TWorker>
    class ArchetypeMixin {
    public:
        typedef TWorker WorkerType;
    protected:
        un::WorkerPool<WorkerType> _pool;
    public:
        ArchetypeMixin() : _pool() {

        }

    protected:
        /**
         * Dispatch a local handle to the worker pool.
         */
        void dispatchLocal(JobHandle handle) {
            _pool.dispatch(handle);
        }

        /**
         * Dispatch a set of local handles to the worker pool.
         */
        void dispatchLocal(std::set<JobHandle> handles) {
            _pool.dispatch(handles);
        }

    public:
        friend class WorkerChain<WorkerType>;

        template<typename>
        friend
        class JobChain;
    };
}
#endif
