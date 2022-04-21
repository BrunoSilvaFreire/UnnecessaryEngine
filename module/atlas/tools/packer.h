#ifndef UNNECESSARYENGINE_PACKER_H
#define UNNECESSARYENGINE_PACKER_H

#include <cxxopts.hpp>
#include <png++/png.hpp>
#include "packer_entry.h"
#include "packer.h"
#include <algorithms/heuristics.h>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/memory/buffer_stream.h>
#include <algorithms/max_rectangles_algorithm.h>
#include <jobs/pack_texture_job.h>
#include <grapphs/dot.h>
#include <sstream>
#include <istream>

namespace un::packer {
    void pack(const std::filesystem::path& output, size_t usedArea, const un::packer::PackingStrategy& strategy);

    template<typename Algorithm>
    void pack(
        const Algorithm& algorithm,
        const std::vector<std::string>& files,
        const std::filesystem::path& output
    ) {
        std::vector<un::packer::PackerEntry> entries;
        size_t usedArea = 0;
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
                usedArea += w * h;
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
        auto strategy = algorithm(entries);
        pack(output, usedArea, strategy);
    }

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
                    auto handles = ParallelForJob<::un::JobWorker> < png::rgba_pixel > ::parallelize(
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
#endif
