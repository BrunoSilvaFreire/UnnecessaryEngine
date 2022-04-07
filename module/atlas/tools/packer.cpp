
#include <cxxopts.hpp>
#include <png++/png.hpp>
#include <unnecessary/def.h>
#include "packer_entry.h"
#include "packer.h"
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/memory/buffer_stream.h>
#include <packing_algorithm.h>
#include <sstream>

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
    void pack(const std::vector<std::string>& files) {
        std::vector<un::packer::PackerEntry> entries;
        for (const auto& path : files) {
            const std::string& filePath = std::filesystem::absolute(path).string();
            std::ifstream fileStream(filePath, std::ios::binary);
            if (!fileStream.is_open() || !fileStream.good()) {
                LOG(WARN) << "Unable to read file " << filePath << ", skipping.";
                continue;
            }
            try {
                png::reader<std::ifstream> reader(fileStream);
                reader.read_info();
                const png::image_info& info = reader.get_image_info();
                u32 w = info.get_width();
                u32 h = info.get_height();

                entries.emplace_back(w, h, path);
            } catch (const std::runtime_error& exception) {
                LOG(INFO) << "Exception " << exception.what() << " while loading file " << path
                          << " (" << filePath << ").";
                continue;
            }

        }
        std::sort(
            entries.begin(),
            entries.end(),
            [](const un::packer::PackerEntry& a, const un::packer::PackerEntry& b) {
                return a.getArea() > b.getArea();
            }
        );
        LOG(INFO) << entries.size() << " entries.";
        for (const auto& item : entries) {
            LOG(INFO) << item.getPath() << ": " << item.getWidth() << "x" << item.getHeight() << " (" << item.getArea()
                      << " pixels)";
        }
        un::packer::PackingAlgorithm* algorithm = nullptr;
        auto strategy = algorithm->operator()(entries);
        {
            un::SimpleJobSystem jobSystem(false);
            {
                un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
                for (const auto& operation : strategy.getOperations()) {
                    un::JobHandle loadImageJob, parseImageJob, packImageJob;
                    auto buf = std::make_shared<un::Buffer>();
                    std::shared_ptr<png::image<png::rgba_pixel>> src = nullptr;
                    chain
                        .immediately<un::LoadFileJob>(&loadImageJob, operation.getPath(), buf.get(), std::ios::binary)
                        .after<un::LambdaJob<>>(
                            loadImageJob, &packImageJob,
                            [src, buf]() {
                                un::BufferStream<> stream(buf);
                                png::image<png::rgba_pixel> image(stream);
                            }
                        );

                }
            }
            jobSystem.start();
            jobSystem.complete();
        }
    }
}