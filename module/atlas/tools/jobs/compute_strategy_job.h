#ifndef UNNECESSARYENGINE_COMPUTE_STRATEGY_JOB_H
#define UNNECESSARYENGINE_COMPUTE_STRATEGY_JOB_H

#include <memory>
#include <utility>
#include <packing_algorithm.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/workers/worker.h>

namespace un {

    class ComputeStrategyJob : un::Job<un::JobWorker> {
    private:
        std::unique_ptr<un::packer::PackingAlgorithm> algorithm;
        std::vector<un::packer::PackerEntry> entries;
        un::packer::PackingStrategy* strategy;
    public:
        ComputeStrategyJob(
            std::unique_ptr<packer::PackingAlgorithm>&& algorithm,
            std::vector<un::packer::PackerEntry> entries,
            un::packer::PackingStrategy* strategy
        ) : algorithm(std::move(algorithm)),
            entries(std::move(entries)),
            strategy(strategy) { }

        void operator()(WorkerType* worker) override {
            *strategy = algorithm->operator()(entries);
        }
    };
}
#endif