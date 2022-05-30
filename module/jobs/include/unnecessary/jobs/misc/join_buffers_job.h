#ifndef UNNECESSARYENGINE_JOIN_BUFFERS_JOB_H
#define UNNECESSARYENGINE_JOIN_BUFFERS_JOB_H

#include <vector>
#include <functional>
#include <numeric>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    class JoinBuffersJob : public un::SimpleJob {
    private:
        std::vector<std::shared_ptr<un::Buffer>> buffers;
        std::shared_ptr<un::Buffer> output;
    public:
        JoinBuffersJob(
            const std::vector<std::shared_ptr<un::Buffer>>& buffers,
            const std::shared_ptr<un::Buffer>& output
        );

        void operator()(WorkerType* worker) override;
    };
}
#endif
