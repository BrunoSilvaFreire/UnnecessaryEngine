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

namespace un {
    template<typename _JobSystemType>
    class JobChain {
    public:
        typedef _JobSystemType JobSystemType;
    private:
        JobSystemType* system;
        std::set<JobHandle> toStart;
        std::set<JobHandle> allJobs;
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


        JobChain& immediately(JobHandle id) {
            allJobs.emplace(id);
            toStart.emplace(id);
            return *this;
        }

        template<typename J>
        JobChain& immediately(J* job) {
            auto handle = system->enqueue(job, false);
            immediately(handle);
            return *this;
        }

        template<typename J>
        JobChain& immediately(JobHandle* id, J* job) {
            JobHandle handle = *id = system->enqueue(job, false);
            immediately(handle);
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

        JobChain& after(JobHandle runAfter, JobHandle job) {
            system->addDependency(runAfter, job);
            toStart.erase(runAfter);
            return *this;
        }

        template<typename J, typename... Args>
        JobChain& after(JobHandle dependencyId, Args... args) {
            after(dependencyId, new J(std::forward<Args>(args)...));
            return *this;
        }


        template<typename T>
        JobChain& after(u32 after, u32* id, T* job) {
            JobHandle handle = *id = system->enqueue(after, job);
            allJobs.emplace(handle);
            return *this;
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


        template<typename T, typename... Args>
        JobChain& after(JobHandle dependencyId, JobHandle* resultId, Args... args) {
            after(dependencyId, resultId, new T(std::forward<Args>(args)...));
            return *this;
        }

        void dispatch(){

        }

        JobSystemType* getSystem() const {
            return system;
        }
    };
}
#endif
