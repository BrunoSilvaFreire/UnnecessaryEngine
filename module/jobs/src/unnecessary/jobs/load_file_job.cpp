#include <unnecessary/jobs/misc/load_file_job.h>
#include <sstream>

namespace un {

    LoadFileJob::LoadFileJob(
        const std::filesystem::path& path,
        un::Buffer* buffer,
        std::ios::openmode openMode
    ) : path(path), openMode(openMode), buffer(buffer) {
        setName(std::string("Load ") + path.string());

        if (!exists(path)) {
            std::stringstream err;
            err << "File at '" << path << "' doesn't exist";
            throw std::runtime_error(err.str());
        }
    }

    void un::LoadFileJob::operator()(un::JobWorker* worker) {
        std::ifstream stream(path, openMode);
        if (!stream.is_open()) {
            return;
        }
        stream.seekg(0, std::ios::end);
        std::size_t fileSize = stream.tellg();
        bool text = isText();
        if (text) {
            fileSize += 1; // Null terminator
        }
        buffer->resize(fileSize);
        buffer->setZero();
        if (text) {
            buffer[fileSize - 1] = 0; // Null terminator
        }
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

    bool LoadFileJob::isText() const {
        return openMode == std::ios::in;
    }
}