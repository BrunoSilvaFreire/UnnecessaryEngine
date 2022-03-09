#ifndef UNNECESSARYENGINE_JOB_SYSTEM_PROFILER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_PROFILER_H

#include <vector>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/profiler/worker_profiler.h>

namespace un {
    template<typename TWorker>
    class ProfilerPool {
    private:
        std::vector<un::WorkerProfiler<TWorker>> profilers;
    public:
        void bootstrap(WorkerPool<TWorker>* fromPool) {
            for (TWorker* item : fromPool->getWorkers()) {
                profilers.emplace_back(item);
            }
        }
    };

    template<typename TJobSystem>
    class JobSystemProfiler {
    private:
        typedef typename TJobSystem::ProfilerPoolTuple ProfilerPoolTuple;
        ProfilerPoolTuple tuple;

    public:
        JobSystemProfiler(TJobSystem* jobSystem) : tuple() {
            TJobSystem::for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    ProfilerPool<TArchetype>& profilerPool = std::get<TArchetypeIndex>(
                        tuple
                    );
                    un::WorkerPool<TArchetype>* workerPool = jobSystem->template getWorkerPool<TArchetype>();
                    profilerPool.bootstrap(workerPool);
                }
            );
        }

        virtual ~JobSystemProfiler() {

        }


        void saveToFile(std::filesystem::path file) {
            TJobSystem::for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    ProfilerPool<TArchetype>& profilerPool = std::get<TArchetypeIndex>(
                        tuple
                    );

                }
            );
        }
    };
}
#endif