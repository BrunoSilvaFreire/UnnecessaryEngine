//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_JOB_CHAIN_H
#define UNNECESSARYENGINE_JOB_CHAIN_H

#include <unordered_map>
#include <unnecessary/jobs/jobs.h>
#include <utility>

namespace un {
    class JobChain {
    private:
        std::unordered_map<Job *, u32> jobIds;
        un::JobSystem *system;
        std::vector<u32> toStart;
        std::vector<u32> allJobs;
        bool dispatchOnDestruct;
    public:
        JobChain(un::JobSystem *system, bool dispatchOnDestruct = true) : system(system), jobIds(),
                                                                          dispatchOnDestruct(dispatchOnDestruct) {

        }

        virtual ~JobChain() {
            if (dispatchOnDestruct) {
                dispatch();
            }
        }


        JobChain &immediately(Job *job) {
            u32 id = system->enqueue(job, false);
            allJobs.push_back(id);
            toStart.push_back(id);
            return *this;
        }

        JobChain &immediately(u32 *id, Job *job) {
            *id = system->enqueue(job, false);
            allJobs.push_back(*id);
            toStart.push_back(*id);
            return *this;
        }

        JobChain &after(u32 dependencyId, Job *job) {
            auto id = system->enqueue(dependencyId, job);
            allJobs.push_back(id);
            return *this;
        }

        JobChain &after(u32 dependencyId, const un::LambdaJob::Callback &callback) {
            after(dependencyId, new un::LambdaJob(callback));
            return *this;
        }

        JobChain &after(u32 after, u32 *id, Job *job) {
            *id = system->enqueue(after, job);
            allJobs.push_back(*id);
            return *this;
        }

        template<typename T, typename ...Args>
        JobChain &immediately(Args... args) {
            return immediately(new T(std::forward<Args>(args)...));
        }

        template<typename T, typename ...Args>
        JobChain &immediately(u32 *id, Args... args) {
            return immediately(id, new T(std::forward<Args>(args)...));
        }

        template<typename T, typename ...Args>
        JobChain &after(u32 dependencyId, Args... args) {
            after(dependencyId, new T(std::forward<Args>(args)...));
            return *this;
        }

        template<typename T, typename ...Args>
        JobChain &after(u32 dependencyId, u32 *resultId, Args... args) {
            after(dependencyId, resultId, new T(std::forward<Args>(args)...));
            return *this;
        }

        void dispatch() {
            for (u32 id : toStart) {
                system->markForExecution(id);
            }
        }

        JobChain &onFinished(const un::LambdaJob::Callback &callback) {
            auto job = new un::LambdaJob(callback);
            auto jobId = system->enqueue(job, false);
            for (const u32 item : allJobs) {
                system->addDependency(jobId, item);
            }
            return *this;
        }

        JobChain &onFinished(const un::LambdaJob::VoidCallback &callback) {
            auto job = new un::LambdaJob(callback);
            auto jobId = system->enqueue(job, false);
            for (const u32 item : allJobs) {
                system->addDependency(jobId, item);
            }
            return *this;
        }
    };
}
#endif
