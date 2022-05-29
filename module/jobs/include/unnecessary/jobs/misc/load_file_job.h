#ifndef UNNECESSARYENGINE_LOAD_FILE_JOB_H
#define UNNECESSARYENGINE_LOAD_FILE_JOB_H

#include <filesystem>
#include <fstream>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    class LoadFileJob : public un::Job<JobWorker> {
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
}
#endif
