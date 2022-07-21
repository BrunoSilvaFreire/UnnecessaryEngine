#ifndef UNNECESSARYENGINE_DYNAMIC_CHAIN_H
#define UNNECESSARYENGINE_DYNAMIC_CHAIN_H

#include <unordered_set>
#include <utility>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>

namespace un {


    template<typename TJobSystem, typename TFunctor>
    class DynamicChain {
    public:
        /**
         * Invoked when a job of the specified type is completed.
         */
        template<typename TArchetype>
        using Handler = std::function<void(
            un::JobHandle handle,
            un::ptr<typename TArchetype::JobType> job,
            un::JobChain<TJobSystem>& subChain
        )>;
        using HandlerTuple = typename TJobSystem::template ArchetypeTuple<Handler>;
    private:
        un::ptr<TJobSystem> _jobSystem;
        std::unordered_set<un::JobHandle> _jobsInFlight;
        std::mutex _inFlightMutex;
        TFunctor _functor;
        std::condition_variable _completed;

        template<typename TArchetype>
        void onJobFinished(un::JobHandle handle, un::ptr<typename TArchetype::JobType> job) {

            std::lock_guard<std::mutex> lock(_inFlightMutex);
            _jobsInFlight.erase(handle);

            un::JobChain<TJobSystem> subChain(_jobSystem);
            _functor.template operator()<TArchetype>(handle, job, subChain);
            include(subChain);

            if (_jobsInFlight.empty()) {
                _completed.notify_all();
            }
        }

    public:
        DynamicChain(
            un::ptr<TJobSystem> jobSystem,
            TFunctor&& functor
        ) : _jobsInFlight(),
            _jobSystem(jobSystem),
            _functor(std::move(functor)) {
        }

        void include(un::JobChain<TJobSystem>& subChain) {
            {
                std::lock_guard<std::mutex> lock(_inFlightMutex);
                const auto& jobs = subChain.getAllJobs();
                TJobSystem::for_each_archetype(
                    [&]<typename TJobArchetype, std::size_t Index>() {
                        const std::set<un::JobHandle>& batch = jobs.template getBatch<TJobArchetype>();
                        if (batch.empty()) {
                            return;
                        }
                        _jobsInFlight.insert(batch.begin(), batch.end());
                        for (un::JobHandle handle : batch) {
                            const auto job = _jobSystem->template getJob<TJobArchetype>(handle);
                            subChain.template after<un::LambdaJob<>>(
                                handle,
                                [this, handle, job]() {
                                    this->onJobFinished<TJobArchetype>(handle, job);
                                }
                            );
                        }
                    }
                );
            }
        }

        template<typename TJob, typename ...TArgs>
        void include(TArgs... args) {
            un::JobChain<TJobSystem> chain(_jobSystem);
            chain.template immediately<TJob>(std::forward<TArgs>(args)...);
            include(chain);
        }

        void wait() {
            std::unique_lock<std::mutex> lock(_inFlightMutex);
            _completed.wait(lock);
        }
    };

    template<typename TJobSystem, typename TFunctor>
    un::DynamicChain<TJobSystem, TFunctor> create_dynamic_chain(
        un::ptr<TJobSystem> jobSystem,
        TFunctor&& functor
    ) {
        return un::DynamicChain<TJobSystem, TFunctor>(jobSystem, std::forward<TFunctor>(functor));
    };
}
#endif
