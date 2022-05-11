#ifndef UNNECESSARYENGINE_ARCHIVER_H
#define UNNECESSARYENGINE_ARCHIVER_H

#include <unnecessary/misc/files.h>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/serialization/serialized.h>

namespace un {
    class Archiver {
    public:
        un::Serialized read_file(const std::filesystem::path& file) {
            un::Buffer buf;
            un::files::read_file_into_buffer(file, buf);
            un::Serialized result;
            read(buf, result);
            return result;
        };


        virtual un::Buffer write(const un::Serialized& serialized) = 0;

        virtual void read(const un::Buffer& buffer, un::Serialized& into) = 0;
    };
}
#endif
