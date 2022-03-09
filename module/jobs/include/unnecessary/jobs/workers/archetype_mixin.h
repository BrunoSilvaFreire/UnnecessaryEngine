//
// Created by brunorbsf on 09/03/22.
//

#ifndef UNNECESSARYENGINE_ARCHETYPE_MIXIN_H
#define UNNECESSARYENGINE_ARCHETYPE_MIXIN_H

#include <unnecessary/jobs/workers/worker_pool.h>

namespace un {
    template<typename TWorker>
    class ArchetypeMixin {
    public:
        typedef TWorker WorkerType;
    private:
        un::WorkerPool<TWorker> _pool;
    public:
        ArchetypeMixin() : _pool(nullptr) {

        }

        template<typename T>
        void dispatch(JobHandle handle);

        template<>
        void dispatch<TWorker>(JobHandle handle) {
            _pool.dispatch(handle);
        }
    };
}
#endif
