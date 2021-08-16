//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
#define UNNECESSARYENGINE_PARALLEL_FOR_JOB_H

#include <unnecessary/jobs/jobs.h>
#include <unnecessary/jobs/job_chain.h>

namespace un {
    class ParallelForJob;

    class ParallelizeJob : public un::Job {
    private:
        un::ParallelForJob* parallelForJob;
        size_t fromIndex, toIndex;
    public:
        ParallelizeJob(
            un::ParallelForJob* parallelForJob,
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

        void schedule(
            un::JobSystem* system,
            size_t numEntries,
            size_t minNumberLoopsPerThread = 64
        );
    };
}
#endif //UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
