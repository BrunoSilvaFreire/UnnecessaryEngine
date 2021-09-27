#include <unnecessary/jobs/job_chain.h>

namespace un {

    JobChain::JobChain(
        un::JobSystem* system,
        bool dispatchOnDestruct
    ) : system(system),
        jobIds(),
        dispatchOnDestruct(dispatchOnDestruct) {

    }

    JobChain::~JobChain() {
        if (dispatchOnDestruct) {
            dispatch();
        }
    }

    JobChain& JobChain::enqueue(u32* id, Job* job) {
        *id = system->enqueue(job, false);
        return *this;
    }

    JobChain& JobChain::immediately(u32 id) {
        allJobs.emplace(id);
        toStart.emplace(id);
        recentlyAdded.emplace(id);
        return *this;
    }

    JobChain& JobChain::immediately(Job* job) {
        u32 id;
        enqueue(&id, job);
        immediately(id);
        return *this;
    }

    JobChain& JobChain::immediately(u32* id, Job* job) {
        *id = system->enqueue(job, false);
        immediately(*id);
        return *this;
    }

    JobChain& JobChain::after(u32 dependencyId, Job* job) {
        auto id = system->enqueue(dependencyId, job);
        allJobs.emplace(id);
        return *this;
    }

    JobChain& JobChain::after(u32 dependencyId, const LambdaJob::Callback& callback) {
        after(dependencyId, new un::LambdaJob(callback));
        return *this;
    }

    JobChain& JobChain::after(u32 runAfter, u32 job) {
        system->addDependency(runAfter, job);
        toStart.erase(runAfter);
        return *this;
    }

    JobChain& JobChain::after(u32 after, u32* id, Job* job) {
        *id = system->enqueue(after, job);
        allJobs.emplace(*id);
        recentlyAdded.emplace(*id);
        return *this;
    }


    void JobChain::dispatch() {
        for (u32 id : toStart) {
            system->markForExecution(id);
        }
    }

    JobChain& JobChain::onFinished(const LambdaJob::Callback& callback) {
        auto job = new un::LambdaJob(callback);
        auto jobId = system->enqueue(job, false);
        for (const u32 item : allJobs) {
            system->addDependency(jobId, item);
        }
        return *this;
    }

    const std::set<u32>& JobChain::getRecentlyAdded() const {
        return recentlyAdded;
    }

    un::Job* JobChain::getJob(u32 i) {
        return system->getJob(i);
    }
}