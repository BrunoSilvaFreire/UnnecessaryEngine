
#include <cxxopts.hpp>
#include <png++/png.hpp>
#include "packer_entry.h"
#include "packer.h"
#include <packing.h>
#include <algorithms/heuristics.h>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/memory/buffer_stream.h>
#include <algorithms/heuristics.h>
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
    options.add_options()
        ("output", "Input file(s) to concatenate", cxxopts::value<std::vector<std::string>>());
    options.parse_positional("input_files");
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto files = result["input_files"].as<std::vector<std::string>>();
        auto out = result["output"].as<std::string>();

        un::packer::pack(
            un::packer::MaxRectanglesAlgorithm<un::bestLongSideFitHeuristic>(),
            files,
            out
        );
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}

#endif

namespace un::packer {

    void pack(const std::filesystem::path& output, size_t usedArea, const un::packer::PackingStrategy& strategy) {
        size_t totalArea = strategy.getWidth() * strategy.getHeight();
        size_t unused = totalArea - usedArea;
        f32 efficiency = (static_cast<f32>(usedArea) / static_cast<f32>(totalArea)) * 100;
        LOG(INFO) << "Packed with " << efficiency << "% efficiency. (packedArea: "
                  << usedArea << ", totalArea: " << totalArea << ", unusedArea: " << unused << ").";
        {
            auto packed = std::__1::make_shared<png::image<png::rgba_pixel>>(
                strategy.getWidth(),
                strategy.getHeight()
            );
            SimpleJobSystem jobSystem(false);
            {
                JobChain<SimpleJobSystem> chain(&jobSystem);
                for (const auto& operation : strategy.getOperations()) {
                    JobHandle loadImageJob, parseImageJob;
                    auto buf = std::__1::make_shared<Buffer>();
                    auto src = std::__1::make_shared<png::image<png::rgba_pixel>>();
                    const std::filesystem::path& imgPath = operation.getPath();
                    chain.immediately<LoadFileJob>(&loadImageJob, imgPath, buf.get(), std::ios::binary)
                        .after<LambdaJob<>>(
                            loadImageJob, &parseImageJob,
                            [src, buf]() {
                                BufferStream<> sbuf(buf);
                                std::istream stream(&sbuf);
                                png::image<png::rgba_pixel> img(stream);
                                *src = img;
                            }
                        );

                    chain.setName(parseImageJob, std::string("Parse Image ") + imgPath.string());
                    const Rect<u32>& rect = operation.getDestination();
                    glm::uvec2 origin = rect.getOrigin();
                    auto* job = new PackTextureJob<png::rgba_pixel>(
                        src,
                        packed,
                        rect
                    );
                    std::stringstream name;
                    name << "Pack image " << imgPath;
                    job->setName(name.str());
                    auto handles = PackTextureJob<png::rgba_pixel>::parallelize(
                        job,
                        chain,
                        rect.getArea(),
                        512
                    );
                    for (JobHandle handle : handles) {
                        chain.after<JobWorker>(parseImageJob, handle);
                    }
                };
            }
            jobSystem.start();
            jobSystem.complete();
            LOG(INFO) << "Result written to " << std::filesystem::absolute(output);
            packed->write(output.string());
        }
    }
}