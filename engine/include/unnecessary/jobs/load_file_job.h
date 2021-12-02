#ifndef UNNECESSARYENGINE_LOAD_FILE_JOB_H
#define UNNECESSARYENGINE_LOAD_FILE_JOB_H

#include <unnecessary/jobs/jobs.h>
#include <filesystem>
#include <fstream>
#include "unnecessary/misc/membuf.h"

namespace un {
    class LoadFileJob : public un::Job {
        std::filesystem::path path;
        std::ios_base::openmode openMode;
        un::Buffer* buffer;
    public:
        LoadFileJob(
            const std::filesystem::path& path,
            un::Buffer* buffer,
            std::ios_base::openmode openMode = std::ios::binary
        ) : path(path), openMode(openMode), buffer(buffer) {}

        void operator()(un::JobWorker* worker) override {
            std::ifstream stream(path, openMode);
            stream.seekg(std::ios::end);
            std::size_t fileSize = stream.tellg();
            buffer->resize(fileSize);
            stream.seekg(std::ios::beg);
            size_t pos = 0;
            while (stream.good()) {
                stream >> buffer->operator[](pos++);
            }
            stream.close();
        }
    };
}
#endif
