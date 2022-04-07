
#include <cxxopts.hpp>
#include <png++/png.hpp>
#include <unnecessary/def.h>
#include "image_rect.h"
#include "packer.h"
#include <unnecessary/logging.h>


#ifndef UN_TESTING

int main(int argc, char* args[]) {
    cxxopts::Options options("UnnecessaryPacker");
    options.add_options()
        ("input_files", "Input file(s) to concatenate", cxxopts::value<std::vector<std::string>>());
    options.parse_positional("input_files");
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto files = result["input_files"].as<std::vector<std::string>>();
        un::packer::pack(files);
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}

#endif

namespace un::packer {
    void pack(std::vector<std::string> files) {
        std::vector<un::PackerEntry> entries;
        for (const auto& path : files) {
            std::ifstream fileStream(path);
            png::reader<std::ifstream> reader(fileStream);
            reader.read_info();
            const png::image_info& info = reader.get_image_info();
            u32 w = info.get_width();
            u32 h = info.get_height();

            entries.emplace_back(w, h, path);
        }
        LOG(INFO) << entries.size() << " entries.";
        for (const auto& item : entries) {
            LOG(INFO) << item.getPath() << ": " << item.getWidth() << "x" << item.getHeight() << " (" << item.getArea()
                      << " pixels)";
        }
    }
}