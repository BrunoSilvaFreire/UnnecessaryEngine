#ifndef UNNECESSARYENGINE_FILE_JOBS_H
#define UNNECESSARYENGINE_FILE_JOBS_H

#include <filesystem>
#include <fstream>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    class load_file_job : public simple_job {
    private:
        std::filesystem::path _path;
        std::ios::openmode _openMode;
        byte_buffer* _buffer;
        using stream_type = std::basic_ifstream<byte_buffer::element_type>;

    public:
        load_file_job(
            const std::filesystem::path& path,
            byte_buffer* buffer,
            std::ios::openmode openMode
        );

        void operator()(job_worker* worker) override;
    };

    class write_file_job : public simple_job {
    private:
        std::filesystem::path _path;
        std::ios::openmode _openMode;
        std::shared_ptr<byte_buffer> _buffer;

    public:
        write_file_job(
            std::filesystem::path path, std::ios::openmode openMode, std::shared_ptr<
            byte_buffer
        > buffer
        );

        void operator()(worker_type* worker) override;
    };
}
#endif
