#ifndef UNNECESSARYENGINE_FILES_H
#define UNNECESSARYENGINE_FILES_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include "unnecessary/memory/membuf.h"

namespace un::files {
    void read_file_into_buffer(
        const std::filesystem::path& path,
        un::Buffer& buffer,
        std::ios::openmode openMode = std::ios::in
    );

    void ensure_directory_exists(const std::filesystem::path& path);

    void safe_write_file(
        const std::filesystem::path& path,
        std::stringstream& stream,
        std::ios::openmode openMode = std::ios::in
    );
}
#endif