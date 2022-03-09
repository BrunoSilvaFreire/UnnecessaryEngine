#ifndef UNNECESSARYENGINE_JOB_DISPATCH_H
#define UNNECESSARYENGINE_JOB_DISPATCH_H

#include <set>
#include <unnecessary/jobs/job.h>
#include <unnecessary/misc/templates.h>

namespace un {
    template<typename TWorker>
    class DispatchBatch {
    protected:
        std::set<un::JobHandle> _batch;
    public:
        void dispatch(un::JobHandle handle) {
            _batch.emplace(handle);
        }

        const std::set<un::JobHandle>& getDispatchBatch() {
            return _batch;
        }
    };

    /**
     *
     */
    template<typename ...Archetypes>
    class JobDispatchTable : public DispatchBatch<Archetypes> ... {
    private:
        un::repeat_tuple<std::set<un::JobHandle>, sizeof...(Archetypes)> _toDispatch;
    public:
        template<typename TWorker>
        void dispatch(un::JobHandle handle) {
            DispatchBatch<TWorker>::dispatch(handle);
        }

        template<typename TWorker>
        const std::set<un::JobHandle>& getDispatchBatch() {
            return DispatchBatch<TWorker>::_batch;
        }
    };

}
#endif
