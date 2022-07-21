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
    template<typename TJobSystem>
    class JobChain {
    public:
        typedef TJobSystem JobSystemType;
        typedef typename JobSystemType::DispatchTable DispatchTable;
    private:
        JobSystemType* system;
        DispatchTable toStart;
        DispatchTable allJobs;
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

        void setDispatchOnDestruct(bool dispatchOnDestruct) {
            JobChain::dispatchOnDestruct = dispatchOnDestruct;
        }

        template<typename TJob>
        JobChain& after(JobHandle afterThis, JobHandle runThis) {
            system->addDependency(afterThis, runThis);
            toStart.template erase<typename TJob::WorkerType>(runThis);
            return *this;
        }

        template<typename TArchetype>
        JobChain& afterAll(JobHandle runThis) {
            for (un::JobHandle handle : allJobs.template getBatch<TArchetype>()) {
                if (handle != runThis) {
                    after<typename TArchetype::JobType>(handle, runThis);
                }
            }
            return *this;
        }

        template<typename TWorker>
        JobChain& immediately(JobHandle id) {
            toStart.template dispatch<TWorker>(id);
            allJobs.template dispatch<TWorker>(id);
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

        template<typename TJob>
        JobChain& after(un::JobHandle afterThis, un::JobHandle* runThis, TJob* job) {
            immediately<TJob>(runThis, job);
            JobHandle handle = *runThis;
            after<TJob>(afterThis, handle);
            return *this;
        }

        template<typename J>
        JobChain& immediately(J* job) {
            std::pair<un::JobHandle, un::JobNode*> pair = system->template create<typename J::WorkerType>(
                job,
                false
            );
            return immediately<typename J::WorkerType>(pair.second->poolLocalIndex);
        }

        template<typename J>
        JobChain& immediately(JobHandle* id, J* job) {
            using WorkerType = typename J::WorkerType;
            std::pair<un::JobHandle, un::JobNode*> pair = system->template create<WorkerType>(
                job,
                false
            );
            JobHandle handle = *id = pair.second->poolLocalIndex;
            immediately<WorkerType>(handle);
            return *this;
        }

        template<typename TJob, typename... Args>
        JobChain& immediately(Args... args) {
            return immediately<TJob>(new TJob(std::forward<Args>(args)...));
        }

        template<typename TJob, typename... Args>
        JobChain& immediately(JobHandle* id, Args... args) {
            return immediately<TJob>(id, new TJob(std::forward<Args>(args)...));
        }

        template<typename TJob>
        JobChain& after(JobHandle dependencyId, TJob* job) {
            un::JobHandle handle;
            immediately(&handle, job);
            after<TJob>(handle, dependencyId);
            return *this;
        }


        template<typename J, typename... Args>
        JobChain& after(JobHandle afterThis, Args... args) {
            after<J>(afterThis, new J(std::forward<Args>(args)...));
            return *this;
        }


        template<typename J, typename... Args>
        JobChain& after(JobHandle afterThis, JobHandle* runThis, Args... args) {
            after<J>(afterThis, runThis, new J(std::forward<Args>(args)...));
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
            JobSystemType::for_each_archetype(
                [this, job]<typename TArchetype, size_t Index>() {
                    afterAll<TArchetype>(job);
                }
            );
        }

        void setName(JobHandle i, const std::string& name) {
            system->setName(i, name);
        }

        DispatchTable getAllJobs() const {
            return allJobs;
        }

        std::size_t getNumJobs() {
            std::size_t n = 0;
            JobSystemType::for_each_archetype(
                [&n, this]<typename TArchetype, size_t Index>() {
                    n += allJobs.template getBatch<TArchetype>().size();
                }
            );
            return n;
        }

    };
}
#endif
