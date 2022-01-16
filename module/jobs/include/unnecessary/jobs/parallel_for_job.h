//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
#define UNNECESSARYENGINE_PARALLEL_FOR_JOB_H

#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/misc/benchmark.h>

namespace un {

    template<typename J, typename Worker>
    class ParallelizeJob : public un::Job<Worker> {
    private:
        J* parallelForJob;
        size_t fromIndex, toIndex;
    public:
        ParallelizeJob(
            J* parallelForJob,
            size_t fromIndex,
            size_t toIndex
        ) : parallelForJob(parallelForJob),
            fromIndex(fromIndex),
            toIndex(toIndex) {

        }

        void operator()(Worker* worker) override {
            for (size_t i = fromIndex; i < toIndex; ++i) {
                parallelForJob->operator()(i, worker);
            }
        }
    };

    template<typename Worker>
    class ParallelForJob {
    public:
        virtual void operator()(size_t index, Worker* worker) = 0;

        template<typename J, typename ChainType>
        static void parallelize(
            J* job,
            ChainType& chain,
            size_t numWorkers,
            size_t numEntries,
            size_t minNumberLoopsPerThread
        ) {
            size_t numEntriesPerJob = numEntries / numWorkers;
            if (numEntriesPerJob < minNumberLoopsPerThread) {
                numEntriesPerJob = minNumberLoopsPerThread;
            }
            size_t numFullJobs = numEntries / numEntriesPerJob;
            size_t totalFullyProcessedLoops = numFullJobs * numEntriesPerJob;
            size_t rest = numEntries - totalFullyProcessedLoops;
            for (size_t i = 0; i < numFullJobs; ++i) {
                size_t from = i * numEntriesPerJob;
                size_t to = (i + 1) * numEntriesPerJob;
                chain.template immediately<ParallelizeJob<J, Worker>>(job, from, to);
            }
            if (rest > 0) {
                chain.template immediately<ParallelizeJob<J, Worker>>(
                    job,
                    rest,
                    numEntries
                );
            }
        }
    };
}
#endif //UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
