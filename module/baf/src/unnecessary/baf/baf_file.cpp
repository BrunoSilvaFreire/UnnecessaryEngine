#include <unnecessary/baf/baf_file.h>
#include <unnecessary/baf/file_reader.h>

namespace un {
    const char* BAFFile::kBafMagic = "LMAO";

    BAFFile::BAFFile(const std::filesystem::path& path) {
        file = std::fopen(reinterpret_cast<const char*>(path.c_str()), "r");
        auto reader = get_reader();
        // Check magic
        char magic[4];
        reader.read<char>(magic, 4);
        if (std::strcmp(magic, kBafMagic) != 0) {
            throw std::runtime_error("Provided path doesn't seem to be a BAF file");
        }
        // Read version
        reader.read(version.data(), kNumBytesForVersion);
        // Read header
        u32 numEntries = reader.read<u32>();
        std::vector<BAFEntryPair> pairs(numEntries);
        reader.read(pairs.data(), numEntries);
        entries.reserve(numEntries);
        for (auto pair : pairs) {
            entries.insert(std::make_pair(pair.uuid, pair.actual));
        }
        blocksStart = reader.tell();
    }

    BAFFile::~BAFFile() {
        std::fclose(file);
    }

    void BAFFile::read_section(const BAFSection& section, Buffer& into, std::size_t intoOffset) {
        std::size_t start = blocksStart + section.origin;
        un::FileHelper reader = get_reader();
        reader.seek(start); // Seek to content start
        auto ptr = into.offset(intoOffset);
        reader.read(
            reinterpret_cast<char*>(ptr),
            static_cast<std::streamsize>(section.length)
        ); // Read into buffer
    }

    const std::array<u8, BAFFile::kNumBytesForVersion>& BAFFile::getVersion() const {
        return version;
    }

}