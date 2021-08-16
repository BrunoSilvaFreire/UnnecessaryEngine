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
        std::unordered_map<Job*, u32> jobIds;
        un::JobSystem* system;
        std::vector<u32> toStart;
        std::vector<u32> allJobs;
        bool dispatchOnDestruct;
    public:
        JobChain(
            un::JobSystem* system,
            bool dispatchOnDestruct = true
        );

        virtual ~JobChain();

        JobChain& enqueue(u32* id, Job* job);

        JobChain& immediately(u32 id);

        JobChain& immediately(Job* job);

        JobChain& immediately(u32* id, Job* job);

        JobChain& after(u32 dependencyId, Job* job);

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
    };
}
#endif
