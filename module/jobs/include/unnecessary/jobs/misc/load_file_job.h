#ifndef UNNECESSARYENGINE_LOAD_FILE_JOB_H
#define UNNECESSARYENGINE_LOAD_FILE_JOB_H

#include <filesystem>
#include <fstream>
#include <unnecessary/misc/membuf.h>
#include "unnecessary/jobs/workers/worker.h"

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
            std::ios::openmode openMode = std::ios::binary
        ) : path(path), openMode(openMode), buffer(buffer) {
            setName(std::string("Load ") + path.string());

            if (!exists(path)) {
                std::stringstream err;
                err << "File at '" << path << "' doesn't exist";
                throw std::runtime_error(err.str());
            }
        }

        void operator()(un::JobWorker* worker) override {
            std::ifstream stream(path, openMode);
            if (!stream.is_open()) {
                return;
            }
            stream.seekg(0, std::ios::end);
            std::size_t fileSize = stream.tellg();
            buffer->resize(fileSize);
            stream.seekg(0, std::ios::beg);
            size_t pos = 0;
            stream.read(reinterpret_cast<char*>(buffer->data()), fileSize);
            switch (stream.rdstate()) {

                case std::ios::badbit:
                    throw std::runtime_error("Bad bit after read");
                    break;
                case std::ios::failbit:
                    throw std::runtime_error("Fail bit after read");
                    break;
                default:
                    break;
            }

            stream.close();
        }
    };
}
#endif
