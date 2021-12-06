//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_JOB_CHAIN_H
#define UNNECESSARYENGINE_JOB_CHAIN_H

#include <unordered_map>
#include <set>
#include <unnecessary/jobs/jobs.h>
#include <utility>

namespace un {
    class JobChain {
    private:
        std::unordered_map<Job*, u32> jobIds;
        un::JobSystem* system;
        std::set<u32> toStart;
        std::set<u32> allJobs;
        std::set<u32> recentlyAdded;
        bool dispatchOnDestruct;
    public:

        JobChain(
            un::JobSystem* system,
            bool dispatchOnDestruct = true
        );

        void separate() {
            recentlyAdded.clear();
        }

        virtual ~JobChain();

        const std::set<u32>& getRecentlyAdded() const;

        JobChain& enqueue(u32* id, Job* job);

        JobChain& immediately(u32 id);

        JobChain& immediately(Job* job);

        JobChain& immediately(u32* id, Job* job);

        JobChain& after(u32 dependencyId, Job* job);

        template<typename T, typename... Args>
        JobChain& after(
            const std::initializer_list<u32>& dependencies,
            Args... args
        ) {
            auto job = new T(std::forward<Args>(args)...);
            auto id = system->enqueue(job);
            for (u32 dependency: dependencies) {
                system->addDependency(dependency, id);
            }
            allJobs.emplace(id);
            return *this;
        }

        JobChain& after(u32 dependencyId, const un::LambdaJob::Callback& callback);

        JobChain& after(u32 runAfter, u32 job);

        JobChain& after(u32 after, u32* id, Job* job);

        template<typename T, typename... Args>
        JobChain& immediately(Args... args) {
            return immediately(new T(std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        JobChain& immediately(u32* id, Args... args) {
            return immediately(id, new T(std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        JobChain& after(u32 dependencyId, Args... args) {
            after(dependencyId, new T(std::forward<Args>(args)...));
            return *this;
        }

        template<typename T, typename... Args>
        JobChain& after(u32 dependencyId, u32* resultId, Args... args) {
            after(dependencyId, resultId, new T(std::forward<Args>(args)...));
            return *this;
        }

        void dispatch();

        JobChain& onFinished(const un::LambdaJob::Callback& callback);

        JobSystem* getSystem() const;

        un::Job* getJob(u32 i);
    };
}
#endif
