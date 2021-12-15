//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_JOB_CHAIN_H
#define UNNECESSARYENGINE_JOB_CHAIN_H

#include <unordered_map>
#include <set>
#include <utility>
#include <unnecessary/def.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/misc/lambda_job.h>

namespace un {
    template<typename _JobSystemType>
    class JobChain {
    public:
        typedef _JobSystemType JobSystemType;
        typedef typename JobSystemType::JobHandleSet JobGroupList;
    private:
        JobSystemType* system;
        JobGroupList toStart;
        JobGroupList allJobs;
        bool dispatchOnDestruct;


    public:

        JobChain(
            JobSystemType* system,
            bool dispatchOnDestruct = true
        ) : system(system),
            dispatchOnDestruct(dispatchOnDestruct) {

        }

        ~JobChain() {
            if (dispatchOnDestruct) {
                dispatch();
            }
        }

        template<size_t ArchetypeIndex>
        JobChain& after(JobHandle runAfter, JobHandle job) {
            system->addDependency(runAfter, job);
            std::get<ArchetypeIndex>(toStart).erase(job);
            return *this;
        }

        template<size_t ArchetypeIndex>
        JobChain& afterAll(JobHandle job) {
            for (un::JobHandle handle : std::get<ArchetypeIndex>(allJobs)) {
                if (handle != job) {
                    after<ArchetypeIndex>(handle, job);
                }
            }
        }

        template<typename Archetype>
        JobChain& after(JobHandle runAfter, JobHandle job) {
            return after<JobSystemType::template index_of_archetype<Archetype>()>(
                runAfter,
                job
            );
        }

        template<size_t ArchetypeIndex>
        JobChain& immediately(JobHandle id) {
            std::get<ArchetypeIndex>(toStart).emplace(id);
            std::get<ArchetypeIndex>(allJobs).emplace(id);
            return *this;
        }

        template<typename Archetype>
        JobChain& immediately(JobHandle id) {
            return immediately<JobSystemType::template index_of_archetype<Archetype>()>(id);
        }

        template<typename T, typename... Args>
        JobChain& after(
            const std::initializer_list<JobHandle>& dependencies,
            Args... args
        ) {
            auto job = new T(std::forward<Args>(args)...);
            auto id = system->enqueue(job);
            for (JobHandle dependency : dependencies) {
                system->addDependency(dependency, id);
            }
            allJobs.emplace(id);
            return *this;
        }

        template<typename T>
        JobChain& after(un::JobHandle run, un::JobHandle* id, T* job) {
            immediately<T>(id, job);
            JobHandle handle = *id;
            after<typename T::WorkerType>(run, handle);
            return *this;
        }

        template<typename J>
        JobChain& immediately(J* job) {
            auto handle = system->enqueue(job, false);
            return immediately<typename J::WorkerType>(handle);
        }

        template<typename J>
        JobChain& immediately(JobHandle* id, J* job) {
            JobHandle handle = *id = system->enqueue(job, false);
            immediately<typename J::WorkerType>(handle);
            return *this;
        }

        template<typename J, typename... Args>
        JobChain& immediately(Args... args) {
            return immediately<J>(new J(std::forward<Args>(args)...));
        }

        template<typename J, typename... Args>
        JobChain& immediately(JobHandle* id, Args... args) {
            return immediately<J>(id, new J(std::forward<Args>(args)...));
        }


        template<typename J, typename... Args>
        JobChain& after(JobHandle dependencyId, Args... args) {
            after(dependencyId, new J(std::forward<Args>(args)...));
            return *this;
        }


        template<typename T, typename... Args>
        JobChain& after(JobHandle dependencyId, JobHandle* resultId, Args... args) {
            after(dependencyId, resultId, new T(std::forward<Args>(args)...));
            return *this;
        }

        void dispatch() {
            system->dispatch(toStart);
        }

        JobSystemType* getSystem() const {
            return system;
        }

        template<typename Worker = un::JobWorker, typename Lambda>
        void finally(Lambda callback) {
            un::JobHandle job;
            immediately<un::LambdaJob<Worker>>(&job, callback);
            un::for_indexed<typename JobSystemType::ArchetypesIndices>(
                [this, job]<size_t Index>() {
                    afterAll<Index>(job);
                }
            );
        }
    };
}
#endif
