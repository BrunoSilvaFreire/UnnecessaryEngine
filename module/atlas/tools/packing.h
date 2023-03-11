#ifndef UNNECESSARYENGINE_PACKING_H
#define UNNECESSARYENGINE_PACKING_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <png++/png.hpp>
#include <packer_entry.h>
#include <algorithm_registry.h>

namespace un {
    void pack(
        simple_job_system& jobSystem,
        const std::vector<packer::packer_entry>& entries
    ) {
        job_chain<simple_job_system> chain(&jobSystem);
        packer::algorithm_registry registry;
        const auto& algorithms = registry.get_algorithms();
        //        std::vector<un::packer::PackingStrategy> strategies(algorithms.size());
        for (const auto& item : algorithms) {
            chain.immediately<lambda_job<>>(
                [&, item]() {
                    const packer::packing_strategy& strategy = item->operator()(entries);
                }
            );
        }
    }

    void pack(
        const std::vector<std::filesystem::path>& files
    ) {
        job_system_builder<simple_job_system> builder;
        auto jobSystem = builder.build();
        std::vector<packer::packer_entry> entries(files.size());
        {
            job_chain<simple_job_system> chain(jobSystem.get());
            std::size_t i = 0;
            for (const auto& file : files) {
                chain.immediately<lambda_job<>>(
                    [&, file, i]() {
                        std::ifstream fileStream(file, std::ios::binary);
                        if (!fileStream.is_open() || !fileStream.good()) {
                            LOG(WARN) << "Unable to read file " << file << ", skipping.";
                            return;
                        }
                        png::reader<std::ifstream> reader(fileStream);
                        reader.read_info();
                        const png::image_info& info = reader.get_image_info();
                        u32 w = info.get_width();
                        u32 h = info.get_height();
                        entries[i] = packer::packer_entry(w, h, file);
                    }
                );
            }
            chain.finally(
                [&]() {
                    LOG(INFO) << "All entries loaded (" << entries.size() << ")";

                    pack(*jobSystem, entries);
                }
            );
        }
        jobSystem->join();
    }
}

#endif
