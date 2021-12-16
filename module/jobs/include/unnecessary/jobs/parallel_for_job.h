//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
#define UNNECESSARYENGINE_PARALLEL_FOR_JOB_H

#include <unnecessary/jobs/jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/misc/benchmark.h>

namespace un {
    class ParallelForJob;

    template<class J>
    class ParallelizeJob : public un::Job {
    private:
        J* parallelForJob;
        size_t fromIndex, toIndex;
    public:
        ParallelizeJob(
            J* parallelForJob,
            size_t fromIndex,
            size_t toIndex
        ) : parallelForJob(
            parallelForJob
        ), fromIndex(fromIndex), toIndex(toIndex) {}

        void operator()(un::JobWorker* worker) override;
    };

    class ParallelForJob {
    public:
        virtual void operator()(size_t index, un::JobWorker* worker) = 0;

        template<typename J>
        static void schedule(
            J* job,
            un::JobSystem* system,
            size_t numEntries,
            size_t minNumberLoopsPerThread = 64
        );

        template<typename J>
        static void schedule(

            J* job,
            un::JobChain& chain,
            size_t numEntries,
            size_t minNumberLoopsPerThread = 64
        );
    };

    template<typename J>
    void un::ParallelizeJob<J>::operator()(un::JobWorker* worker) {
        for (size_t i = fromIndex; i < toIndex; ++i) {
            parallelForJob->operator()(i, worker);
        }
    }

    template<typename J>
    void un::ParallelForJob::schedule(
        J* job,
        un::JobSystem* system,
        size_t numEntries,
        size_t minNumberLoopsPerThread
    ) {
        un::JobChain chain(system);
        schedule(job, chain, numEntries, minNumberLoopsPerThread);
    }

    template<typename J>
    void un::ParallelForJob::schedule(
        J* job,
        un::JobChain& chain,
        size_t numEntries,
        size_t minNumberLoopsPerThread
    ) {
        size_t numWorkers = chain.getSystem()->getNumWorkers();
        size_t numEntriesPerJob = numEntries / numWorkers;
        auto start = std::chrono::high_resolution_clock::now();
        if (numEntriesPerJob < minNumberLoopsPerThread) {
            numEntriesPerJob = minNumberLoopsPerThread;
        }
        size_t numFullJobs = numEntries / numEntriesPerJob;
        size_t totalFullyProcessedLoops = numFullJobs * numEntriesPerJob;
        size_t rest = numEntries - totalFullyProcessedLoops;
        /*LOG(INFO) << "Using " << GREEN(numEntriesPerJob) << " per job, over "
                  << GREEN(numFullJobs) << " jobs with "
                  << GREEN(rest) << " remaining.";*/
        for (size_t i = 0; i < numFullJobs; ++i) {
            size_t from = i * numEntriesPerJob;
            size_t to = (i + 1) * numEntriesPerJob;
            chain.immediately<ParallelizeJob<J>>(job, from, to);

        }
        if (rest > 0) {
            chain.immediately<ParallelizeJob<J>>(job, rest, numEntries);
        }
    }

}
#endif //UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
