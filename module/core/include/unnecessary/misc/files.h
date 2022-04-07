#ifndef UNNECESSARYENGINE_FILES_H
#define UNNECESSARYENGINE_FILES_H

#include <filesystem>
#include <fstream>
#include "unnecessary/memory/membuf.h"

namespace un::files {
    void read_file_into_buffer(
        const std::filesystem::path& path,
        un::Buffer& buffer,
        std::ios::openmode openMode = std::ios::in
    );
}
#endif