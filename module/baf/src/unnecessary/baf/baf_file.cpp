#include <unnecessary/baf/baf_file.h>
#include <unnecessary/baf/file_reader.h>

namespace un {
    const char* baf_file::kBafMagic = "LMAO";

    baf_file::baf_file(const std::filesystem::path& path) {
        _file = std::fopen(reinterpret_cast<const char*>(path.c_str()), "r");
        auto reader = get_reader();
        // Check magic
        char magic[4];
        reader.read<char>(magic, 4);
        if (std::strcmp(magic, kBafMagic) != 0) {
            throw std::runtime_error("Provided path doesn't seem to be a BAF file");
        }
        // Read version
        reader.read(_version.data(), kNumBytesForVersion);
        // Read header
        u32 numEntries = reader.read<u32>();
        std::vector<baf_entry_pair> pairs(numEntries);
        reader.read(pairs.data(), numEntries);
        _entries.reserve(numEntries);
        for (auto pair : pairs) {
            _entries.insert(std::make_pair(pair.uuid, pair.actual));
        }
        _blocksStart = reader.tell();
    }

    baf_file::~baf_file() {
        std::fclose(_file);
    }

    void
    baf_file::read_section(const baf_section& section, byte_buffer& into, std::size_t intoOffset) {
        std::size_t start = _blocksStart + section.origin;
        file_helper reader = get_reader();
        reader.seek(start); // Seek to content start
        auto ptr = into.offset(intoOffset);
        reader.read(
            reinterpret_cast<char*>(ptr),
            static_cast<std::streamsize>(section.length)
        ); // Read into buffer
    }

    const std::array<u8, baf_file::kNumBytesForVersion>& baf_file::get_version() const {
        return _version;
    }
}
