
#include <unnecessary/jobs/job_dispatch.h>

namespace un {

    void DispatchBatch::dispatch(JobHandle handle) {
        _batch.emplace(handle);
    }

    void DispatchBatch::erase(JobHandle handle) {
        _batch.erase(handle);
    }

    const std::set<JobHandle>& DispatchBatch::getBatch() const {
        return _batch;
    }
}