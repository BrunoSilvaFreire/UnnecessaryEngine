#ifndef UNNECESSARYENGINE_JOB_DISPATCH_H
#define UNNECESSARYENGINE_JOB_DISPATCH_H

#include <set>
#include <unnecessary/jobs/job.h>
#include <unnecessary/misc/templates.h>

namespace un {
    class DispatchBatch {
    protected:
        std::set<un::JobHandle> _batch;
    public:
        void dispatch(JobHandle handle);

        void erase(JobHandle handle);

        const std::set<JobHandle>& getBatch() const;
    };

    template<typename TWorker>
    class DispatchBatchMixin : public DispatchBatch {};

    template<typename ...Archetypes>
    class JobDispatchTable : private DispatchBatchMixin<Archetypes> ... {
    public:
        template<typename TWorker>
        void dispatch(un::JobHandle handle) {
            DispatchBatchMixin<TWorker>::dispatch(handle);
        }

        template<typename TWorker>
        void erase(un::JobHandle handle) {
            DispatchBatchMixin<TWorker>::erase(handle);
        }

        template<typename TWorker>
        const std::set<un::JobHandle>& getBatch() const {
            return DispatchBatchMixin<TWorker>::getBatch();
        }
    };

}
#endif
