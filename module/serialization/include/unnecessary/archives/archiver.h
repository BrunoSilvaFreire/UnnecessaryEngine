#ifndef UNNECESSARYENGINE_ARCHIVER_H
#define UNNECESSARYENGINE_ARCHIVER_H

#include <unnecessary/misc/files.h>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/serialization/serialized.h>

namespace un {
    class archiver {
    public:
        serialized read_file(const std::filesystem::path& file) {
            byte_buffer buf;
            files::read_file_into_buffer(file, buf);
            serialized result;
            read(buf, result);
            return result;
        };

        virtual byte_buffer write(const serialized& serialized) = 0;

        virtual void read(const byte_buffer& buffer, serialized& into) = 0;
    };
}
#endif
