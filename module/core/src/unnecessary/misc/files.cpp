#include <unnecessary/misc/files.h>

namespace un::files {

    void read_file_into_buffer(const std::filesystem::path& path, Buffer& buffer, std::ios::openmode openMode) {
        std::ifstream stream(path, openMode);
        if (!stream.is_open()) {
            return;
        }
        stream.seekg(0, std::ios::end);
        std::size_t fileSize = stream.tellg();
        bool text = openMode == std::ios::in;
        if (text) {
            fileSize += 1; // Null terminator
        }
        buffer.resize(fileSize);
        buffer.setZero();
        if (text) {
            buffer.operator[](fileSize - 1) = 0; // Null terminator
        }
        stream.seekg(0, std::ios::beg);
        size_t pos = 0;
        stream.read(
            reinterpret_cast<char*>(buffer.data()),
            static_cast<std::streamsize>(fileSize)
        );
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
}