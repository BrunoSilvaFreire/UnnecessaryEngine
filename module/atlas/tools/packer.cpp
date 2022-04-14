
#include <cxxopts.hpp>
#include <png++/png.hpp>
#include <unnecessary/def.h>
#include "packer_entry.h"
#include "packer.h"
#include "unnecessary/jobs/recorder/job_system_recorder.h"
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/memory/buffer_stream.h>
#include <algorithms/max_rectangles_algorithm.h>
#include <jobs/pack_texture_job.h>
#include <grapphs/dot.h>
#include <sstream>
#include <istream>

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
        un::packer::MaxRectanglesAlgorithm algorithm(
            glm::uvec2(
                std::numeric_limits<u32>::max(),
                std::numeric_limits<u32>::max()
            )
        );
        auto strategy = algorithm(entries);
        {
            auto packed = std::make_shared<png::image<png::rgba_pixel>>(
                strategy.getWidth(),
                strategy.getHeight()
            );
            un::SimpleJobSystem jobSystem(4, false);
            un::JobSystemRecorder<decltype(jobSystem)> recorder(&jobSystem);
            {
                un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
                for (const auto& operation : strategy.getOperations()) {
                    un::JobHandle loadImageJob, parseImageJob;
                    auto buf = std::make_shared<un::Buffer>();
                    auto src = std::make_shared<png::image<png::rgba_pixel>>();
                    const std::filesystem::path& imgPath = operation.getPath();
                    chain.immediately<un::LoadFileJob>(&loadImageJob, imgPath, buf.get(), std::ios::binary)
                        .after<un::LambdaJob<>>(
                            loadImageJob, &parseImageJob,
                            [src, buf]() {
                                un::BufferStream<> sbuf(buf);
                                std::istream stream(&sbuf);
                                png::image<png::rgba_pixel> img(stream);
                                *src = img;
                            }
                        );

                    chain.setName(parseImageJob, std::string("Parse Image ") + imgPath.string());
                    const Rect<u32>& rect = operation.getDestination();
                    glm::uvec2 origin = rect.getOrigin();
                    auto* job = new un::PackTextureJob<png::rgba_pixel>(
                        src,
                        packed,
                        rect
                    );
                    std::stringstream name;
                    name << "Pack image " << imgPath;
                    job->setName(name.str());
                    auto handles = un::PackTextureJob<png::rgba_pixel>::parallelize(
                        job,
                        chain,
                        rect.getArea(),
                        512
                    );
                    for (un::JobHandle handle : handles) {
                        chain.after<un::JobWorker>(parseImageJob, handle);
                    }
                };
            }
            jobSystem.start();
            jobSystem.complete();
            recorder.saveToFile("packing.csv");
            std::filesystem::path path = std::filesystem::absolute("packed.png");
            packed->write(path.string());
            LOG(INFO) << "Result written to " << path;
        }
    }
}