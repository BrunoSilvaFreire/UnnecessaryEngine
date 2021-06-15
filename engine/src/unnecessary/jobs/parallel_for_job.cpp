//
// Created by bruno on 14/06/2021.
//
#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/misc/benchmark.h>

void un::ParallelizeJob::operator()(un::JobWorker *worker) {
    for (size_t i = fromIndex; i < toIndex; ++i) {
        parallelForJob->operator()(i, worker);
    }
}

void un::ParallelForJob::schedule(un::JobSystem *system, size_t numEntries, size_t minNumberLoopsPerThread) {
    un::Chronometer<std::chrono::microseconds> chronometer;
    size_t numWorkers = system->getNumWorkers();
    size_t numEntriesPerJob = numEntries / numWorkers;
    auto start = std::chrono::high_resolution_clock::now();
    if (numEntriesPerJob < minNumberLoopsPerThread) {
        numEntriesPerJob = minNumberLoopsPerThread;
    }
    un::JobChain chain(system);
    size_t numFullJobs = numEntries / numEntriesPerJob;
    size_t totalFullyProcessedLoops = numFullJobs * numEntriesPerJob;
    size_t rest = numEntries - totalFullyProcessedLoops;
    LOG(INFO) << "Using " << GREEN(numEntriesPerJob) << " per job, over " << GREEN(numFullJobs) << " jobs with "
              << GREEN(rest) << " remaining.";
    for (size_t i = 0; i < numFullJobs; ++i) {
        size_t from = i * numEntriesPerJob;
        size_t to = (i + 1) * numEntriesPerJob;
        chain.immediately<ParallelizeJob>(this, from, to);

    }
    if (rest > 0) {
        chain.immediately<ParallelizeJob>(this, rest, numEntries);
    }
    chain.onFinished([chronometer]() {
        LOG(INFO) << "Parallel job finished after " << chronometer.stop() << " microsecond";
    });

}
