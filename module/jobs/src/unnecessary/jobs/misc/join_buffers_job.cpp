#include <unnecessary/jobs/misc/join_buffers_job.h>

namespace un {
    void join_buffers_job::operator()(worker_type* worker) {
        std::size_t newSize = std::accumulate(
            _buffers.begin(),
            _buffers.end(),
            0,
            [](std::size_t a, const std::shared_ptr<byte_buffer>& b) {
                return a + b->size();
            }
        );
        _output->resize(newSize);
        std::size_t offset = 0;
        for (const auto& buffer : _buffers) {
            _output->copy_from(
                buffer->data(),
                offset,
                buffer->size()
            );
            offset += buffer->size();
        }
    }

    join_buffers_job::join_buffers_job(
        const std::vector<std::shared_ptr<byte_buffer>>& buffers,
        const std::shared_ptr<byte_buffer>& output
    ) : _buffers(buffers), _output(output) {
    }
}
