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
    struct BAFSection {
        std::size_t origin;
        std::size_t length;
    };

    struct BAFEntry {
        BAFSection bounds;
    };
    struct BAFEntryPair {
        uuids::uuid uuid;
        BAFEntry actual;
    };


    class BAFFile {
    private:
        const static char* kBafMagic;
        constexpr static std::size_t kNumBytesForVersion = 8;
        std::FILE* file;
        std::array<u8, kNumBytesForVersion> version;
        std::size_t blocksStart;
        std::unordered_map<uuids::uuid, BAFEntry> entries;

        un::FileHelper get_reader() {
            return std::ifstream(file);
        }

    public:
        BAFFile(const std::filesystem::path& path);

        virtual ~BAFFile();

        const std::array<u8, kNumBytesForVersion>& getVersion() const;

        std::string getVersionString() const {
            return std::string(reinterpret_cast<const char*>(version.data()), kNumBytesForVersion);
        }

        void read_section(const un::BAFSection& section, un::Buffer& into, std::size_t intoOffset);
    };
}
#endif