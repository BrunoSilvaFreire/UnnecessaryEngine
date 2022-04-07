#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/misc/files.h>
#include <sstream>

namespace un {

    LoadFileJob::LoadFileJob(
        const std::filesystem::path& path,
        un::Buffer* buffer,
        std::ios::openmode openMode
    ) : path(path), openMode(openMode), buffer(buffer) {
        setName(std::string("Load ") + path.string());
    }

    void un::LoadFileJob::operator()(un::JobWorker* worker) {
        if (!exists(path)) {
            std::stringstream err;
            err << "File at '" << path << "' doesn't exist";
            throw std::runtime_error(err.str());
        }
        un::files::read_file_into_buffer(path, *buffer, openMode);
    }
}