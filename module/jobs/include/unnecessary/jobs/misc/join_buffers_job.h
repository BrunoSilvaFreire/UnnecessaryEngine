#ifndef UNNECESSARYENGINE_JOIN_BUFFERS_JOB_H
#define UNNECESSARYENGINE_JOIN_BUFFERS_JOB_H

#include <vector>
#include <functional>
#include <numeric>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    class join_buffers_job : public simple_job {
    private:
        std::vector<std::shared_ptr<byte_buffer>> _buffers;
        std::shared_ptr<byte_buffer> _output;

    public:
        join_buffers_job(
            const std::vector<std::shared_ptr<byte_buffer>>& buffers,
            const std::shared_ptr<byte_buffer>& output
        );

        void operator()(worker_type* worker) override;
    };
}
#endif
