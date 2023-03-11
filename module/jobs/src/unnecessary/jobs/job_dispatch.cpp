#include <unnecessary/jobs/job_dispatch.h>

namespace un {
    void dispatch_batch::dispatch(job_handle handle) {
        _batch.emplace(handle);
    }

    void dispatch_batch::erase(job_handle handle) {
        _batch.erase(handle);
    }

    const std::set<job_handle>& dispatch_batch::get_batch() const {
        return _batch;
    }
}
