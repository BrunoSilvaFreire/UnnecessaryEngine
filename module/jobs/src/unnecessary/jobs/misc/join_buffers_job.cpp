#include <unnecessary/jobs/misc/join_buffers_job.h>

namespace un {

    void JoinBuffersJob::operator()(WorkerType* worker) {
        std::size_t newSize = std::accumulate(
            buffers.begin(),
            buffers.end(),
            0,
            [](std::size_t a, const std::shared_ptr<un::Buffer>& b) {
                return a + b->size();
            }
        );
        output->resize(newSize);
        std::size_t offset = 0;
        for (const auto& buffer : buffers) {
            output->copyFrom(
                buffer->data(),
                offset,
                buffer->size()
            );
            offset += buffer->size();
        }
    }

    JoinBuffersJob::JoinBuffersJob(
        const std::vector<std::shared_ptr<un::Buffer>>& buffers,
        const std::shared_ptr<un::Buffer>& output
    ) : buffers(buffers), output(output) { }
}