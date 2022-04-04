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

            std::stringstream str;
            str << "Parallelize " << parallelForJob->getName();
            str << "(" << fromIndex << " -> " << toIndex << ")";
            un::Job<Worker>::name = str.str();
        }

        void operator()(Worker* worker) override {
            parallelForJob->batchStarted(fromIndex, toIndex);
            for (size_t i = fromIndex; i < toIndex; ++i) {
                parallelForJob->operator()(i, worker);
            }
            parallelForJob->batchFinished(fromIndex, toIndex);
        }
    };

    template<typename TWorker>
    class ParallelForJob {
        std::string name = "Unnamed Job";
    public:
        ParallelForJob() {
            name = un::type_name_of(typeid(this));
        }

        const std::string& getName() const {
            return name;
        }

        void setName(const std::string& newName) {
            name = newName;
        }

        /**
         * Called before a batch is processed by a parallelize job
         * @param start The batch start
         * @param end The batch end
         */
        virtual void batchStarted(size_t start, size_t end) {};

        virtual void batchFinished(size_t start, size_t end) {};

        virtual void operator()(size_t index, TWorker* worker) = 0;

        template<typename J, typename ChainType>
        static void parallelize(
            J* job,
            ChainType& chain,
            size_t numEntries,
            size_t minNumberLoopsPerThread
        ) {
            using JobSystemType = typename ChainType::JobSystemType;
            JobSystemType* system = chain.getSystem();
            auto& workerPool = system->template getWorkerPool<TWorker>();
            size_t numWorkers = workerPool.getNumWorkers();
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
                chain.template immediately<ParallelizeJob<J, TWorker>>(job, from, to);
            }
            if (rest > 0) {
                chain.template immediately<ParallelizeJob<J, TWorker>>(
                    job,
                    numEntries - rest,
                    numEntries
                );
            }
        }
    };
}
#endif //UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
