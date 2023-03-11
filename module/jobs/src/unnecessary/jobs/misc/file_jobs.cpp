#include "unnecessary/jobs/misc/file_jobs.h"
#include "unnecessary/misc/files.h"
#include <sstream>
#include <utility>

namespace un {
    load_file_job::load_file_job(
        const std::filesystem::path& path,
        byte_buffer* buffer,
        std::ios::openmode openMode
    ) : _path(path), _openMode(openMode), _buffer(buffer) {
        set_name(std::string("Load ") + path.string());
    }

    void load_file_job::operator()(job_worker* worker) {
        if (!exists(_path)) {
            std::stringstream err;
            err << "File at '" << _path << "' doesn't exist";
            throw std::runtime_error(err.str());
        }
        files::read_file_into_buffer(_path, *_buffer, _openMode);
    }

    write_file_job::write_file_job(
        std::filesystem::path path,
        std::ios::openmode openMode,
        std::shared_ptr<byte_buffer> buffer
    ) : _path(std::move(path)), _openMode(openMode), _buffer(std::move(buffer)) {
        set_name(std::string("Write ") + path.string());
    }

    void write_file_job::operator()(job_worker* worker) {
        files::ensure_directory_exists(_path.parent_path());
        if (!files::write_buffer_into_file(_path, *_buffer, _openMode)) {
            throw std::runtime_error("Unable to save");
        }
    }
}
