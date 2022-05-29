#ifndef UNNECESSARYENGINE_PACKING_H
#define UNNECESSARYENGINE_PACKING_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <png++/png.hpp>
#include <packer_entry.h>
#include <algorithm_registry.h>

namespace un {
    void pack(
        SimpleJobSystem& jobSystem,
        const std::vector<un::packer::PackerEntry>& entries
    ) {
        un::JobChain<SimpleJobSystem> chain(&jobSystem);
        un::packer::AlgorithmRegistry registry;
        const auto & algorithms = registry.getAlgorithms();
//        std::vector<un::packer::PackingStrategy> strategies(algorithms.size());
        for (const auto& item : algorithms) {
            chain.immediately<un::LambdaJob<>>([&, item]() {
                const packer::PackingStrategy& strategy = item->operator()(entries);
            });
        }

    }

    void pack(
        const std::vector<std::filesystem::path>& files
    ) {
        SimpleJobSystem jobSystem(true);
        std::vector<un::packer::PackerEntry> entries(files.size());
        {
            un::JobChain<SimpleJobSystem> chain(&jobSystem);
            std::size_t i = 0;
            for (const auto& file : files) {
                chain.immediately<un::LambdaJob<>>([&, file, i]() {
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
                    entries[i] = un::packer::PackerEntry(w, h, file);
                });
            }
            chain.finally([&]() {
                LOG(INFO) << "All entries loaded (" << entries.size() << ")";
                pack(jobSystem, entries);
            });
        }
        jobSystem.join();
    }
}

#endif
