#ifndef UNNECESSARYENGINE_BAF_FILE_H
#define UNNECESSARYENGINE_BAF_FILE_H

#include <fstream>
#include <cstdio>
#include <filesystem>
#include <uuid.h>
#include <string>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/baf/file_reader.h>

namespace un {
    struct baf_section {
        std::size_t origin;
        std::size_t length;
    };

    struct baf_entry {
        baf_section bounds;
    };

    struct baf_entry_pair {
        uuids::uuid uuid;
        baf_entry actual;
    };

    class baf_file {
    private:
        const static char* kBafMagic;
        constexpr static std::size_t kNumBytesForVersion = 8;
        std::FILE* _file;
        std::array<u8, kNumBytesForVersion> _version;
        std::size_t _blocksStart;
        std::unordered_map<uuids::uuid, baf_entry> _entries;

        file_helper get_reader() {
            return un::file_helper((std::ifstream(reinterpret_cast<const char*>(_file))));
        }

    public:
        baf_file(const std::filesystem::path& path);

        virtual ~baf_file();

        const std::array<u8, kNumBytesForVersion>& get_version() const;

        std::string get_version_string() const {
            return std::string(reinterpret_cast<const char*>(_version.data()), kNumBytesForVersion);
        }

        void read_section(const baf_section& section, byte_buffer& into, std::size_t intoOffset);
    };
}
#endif
