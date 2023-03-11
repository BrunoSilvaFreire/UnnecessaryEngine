#include <cxxopts.hpp>
#include <png++/png.hpp>
#include "packer_entry.h"
#include "packer.h"
#include <packing.h>
#include <algorithms/heuristics.h>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/file_jobs.h>
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
               (
                   "input_files",
                   "RichInput file(s) to concatenate",
                   cxxopts::value<std::vector<std::string>>());
    options.add_options()
               (
                   "output",
                   "RichInput file(s) to concatenate",
                   cxxopts::value<std::vector<std::string>>());
    options.parse_positional("input_files");
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto files = result["input_files"].as<std::vector<std::string>>();
        auto out = result["output"].as<std::string>();

        pack(
            un::packer::max_rectangles_algorithm<un::best_long_side_fit_heuristic>(),
            files,
            out
        );
    }
    catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}

#endif

namespace un::packer {
    void
    pack(const std::filesystem::path& output, size_t usedArea, const packing_strategy& strategy) {
        size_t totalArea = strategy.get_width() * strategy.get_height();
        size_t unused = totalArea - usedArea;
        f32 efficiency = (static_cast<f32>(usedArea) / static_cast<f32>(totalArea)) * 100;
        LOG(INFO) << "Packed with " << efficiency << "% efficiency. (packedArea: "
                  << usedArea << ", totalArea: " << totalArea << ", unusedArea: " << unused << ").";
        {
            auto packed = std::make_shared<png::image<png::rgba_pixel>>(
                strategy.get_width(),
                strategy.get_height()
            );
            job_system_builder<simple_job_system> builder;
            auto jobSystem = builder.build();
            {
                job_chain<simple_job_system> chain(jobSystem.get());
                for (const auto& operation : strategy.get_operations()) {
                    job_handle loadImageJob, parseImageJob;
                    auto buf = std::make_shared<byte_buffer>();
                    auto src = std::make_shared<png::image<png::rgba_pixel>>();
                    const std::filesystem::path& imgPath = operation.get_path();
                    chain.immediately<load_file_job>(
                             &loadImageJob,
                             imgPath,
                             buf.get(),
                             std::ios::binary
                         )
                         .after<lambda_job<>>(
                             loadImageJob, &parseImageJob,
                             [src, buf]() {
                                 buffer_stream<> sbuf(buf);
                                 std::istream stream(&sbuf);
                                 png::image<png::rgba_pixel> img(stream);
                                 *src = img;
                             }
                         );

                    chain.set_name(parseImageJob, std::string("Parse Image ") + imgPath.string());
                    const rect<u32>& rect = operation.get_destination();
                    glm::uvec2 origin = rect.get_origin();
                    auto* job = new pack_texture_job<png::rgba_pixel>(
                        src,
                        packed,
                        rect
                    );
                    std::stringstream name;
                    name << "Pack image " << imgPath;
                    job->set_name(name.str());
                    auto handles = pack_texture_job<png::rgba_pixel>::parallelize(
                        job,
                        chain,
                        rect.get_area(),
                        512
                    );
                    for (job_handle handle : handles) {
                        chain.after<simple_job>(parseImageJob, handle);
                    }
                }
            }
            jobSystem->start();
            jobSystem->complete();
            LOG(INFO) << "Result written to " << absolute(output);
            packed->write(output.string());
        }
    }
}
