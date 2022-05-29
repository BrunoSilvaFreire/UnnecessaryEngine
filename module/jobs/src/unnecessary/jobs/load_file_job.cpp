#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/misc/files.h>
#include <sstream>
#include <utility>

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

    WriteFileJob::WriteFileJob(std::filesystem::path path, std::ios::openmode openMode, Buffer* buffer)
        : path(std::move(path)), openMode(openMode), buffer(buffer) { }

    void WriteFileJob::operator()(JobWorker* worker) {
        un::files::ensure_directory_exists(path.parent_path());
        if (!un::files::write_buffer_into_file(path, *buffer, openMode)) {
            throw std::runtime_error("Unable to save");
        }
    }
}