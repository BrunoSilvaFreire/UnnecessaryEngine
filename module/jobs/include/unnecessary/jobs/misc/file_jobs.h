#ifndef UNNECESSARYENGINE_FILE_JOBS_H
#define UNNECESSARYENGINE_FILE_JOBS_H

#include <filesystem>
#include <fstream>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    class LoadFileJob : public un::SimpleJob {
    private:
        std::filesystem::path path;
        std::ios::openmode openMode;
        un::Buffer* buffer;
        typedef std::basic_ifstream<un::Buffer::ElementType> StreamType;

    public:
        LoadFileJob(
            const std::filesystem::path& path,
            un::Buffer* buffer,
            std::ios::openmode openMode
        );

        void operator()(un::JobWorker* worker) override;
    };

    class WriteFileJob : public un::SimpleJob {
    private:
        std::filesystem::path path;
        std::ios::openmode openMode;
        std::shared_ptr<un::Buffer> buffer;
    public:
        WriteFileJob(std::filesystem::path path, std::ios::openmode openMode, std::shared_ptr<Buffer> buffer);

        void operator()(WorkerType* worker) override;
    };
}
#endif
