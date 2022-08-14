#ifndef UNNECESSARYENGINE_DYNAMIC_CHAIN_H
#define UNNECESSARYENGINE_DYNAMIC_CHAIN_H

#include <unordered_set>
#include <utility>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>

namespace un {

    template<typename TJobSystem>
    class DynamicFlow;

    template<typename TJobSystem>
    class DynamicChain;

    template<typename TJobSystem>
    using Dynamic = std::function<void(un::DynamicChain<TJobSystem>& chain)>;

    template<typename TJobSystem>
    class DynamicChain : public un::JobChain<TJobSystem> {
    private:
        un::ptr<un::DynamicFlow<TJobSystem>> _flow;
        std::unordered_map<un::JobHandle, un::Dynamic<TJobSystem>> _dynamics;
    public:
        explicit DynamicChain(
            un::ptr<DynamicFlow<TJobSystem>> flow,
            bool dispatchOnDispose = true
        );

        DynamicChain(const un::DynamicChain<TJobSystem>&) = delete;

        DynamicChain(un::DynamicChain<TJobSystem>&& other) noexcept
            : un::JobChain<TJobSystem>(std::move(other)),
              _flow(std::move(other._flow)),
              _dynamics(std::move(other._dynamics)) {
            other.dispatchOnDestruct = false;

        };

        ~DynamicChain() override;

        DynamicChain&& dynamic(un::JobHandle handle, Dynamic<TJobSystem> dyn);

        template<typename TJob, typename ...TArgs>
        un::DynamicChain<TJobSystem>& enqueue(un::Dynamic<TJobSystem> dyn, TArgs... args) {
            un::JobHandle handle;
            this->template immediately<TJob>(&handle, args...);
            dynamic(handle, dyn);
            return *this;
        }

        friend class DynamicFlow<TJobSystem>;
    };

    template<typename TJobSystem>
    class DynamicFlow {
    public:
        friend class DynamicChain<TJobSystem>;
    private:
        un::ptr<TJobSystem> _jobSystem;
        std::mutex _inFlightMutex;
        std::unordered_set<un::JobHandle> _pendingJobs;
        std::unordered_map<un::JobHandle, un::Dynamic<TJobSystem>> _dynamics;
        //
        std::condition_variable _completed;

        void include(un::DynamicChain<TJobSystem>& chain, bool lock) {
            if (lock) {
                _inFlightMutex.lock();
            }
            const auto& toAdd = chain._dynamics;
            _dynamics.insert(toAdd.begin(), toAdd.end());
            for (const auto& item : toAdd) {
                JobHandle handle = item.first;
                _pendingJobs.insert(handle);
                JobHandle dynHandle;
                chain.template after<LambdaJob<>>(
                    handle,
                    &dynHandle,
                    [this, handle]() {
                        onJobFinished(handle);
                    }
                );
                std::stringstream ss;
                ss << "Invoke dynamic for job " << handle << ".";
                chain.setName(dynHandle, ss.str());
            }

            if (lock) {
                _inFlightMutex.unlock();
            }
        }

        void onJobFinished(un::JobHandle handle) {

            std::lock_guard<std::mutex> lock(_inFlightMutex);
            _pendingJobs.erase(handle);
            auto iterator = _dynamics.find(handle);
            if (iterator != _dynamics.end()) {
                auto& dynamic = iterator->second;
                un::DynamicChain<TJobSystem> subChain(this, false);
                dynamic(subChain);
                include(subChain, false);
                /* const auto& jobs = subChain.getAllJobs();
                 TJobSystem::for_each_archetype(
                     [&]<typename TJobArchetype, std::size_t Index>() {
                         const std::set<un::JobHandle>& batch = jobs.template getBatch<TJobArchetype>();
                         if (batch.empty()) {
                             return;
                         }
                         _pendingJobs.insert(batch.begin(), batch.end());
                         for (un::JobHandle handle : batch) {
                             subChain.template after<un::LambdaJob<>>(
                                 handle,
                                 [this, handle]() {
                                     this->onJobFinished(handle);
                                 }
                             );
                         }
                     }
                 );*/
            }

            if (_pendingJobs.empty()) {
                _completed.notify_all();
            }
        }

    public:
        DynamicFlow(
            un::ptr<TJobSystem> jobSystem
        ) : _pendingJobs(),
            _jobSystem(jobSystem) {
        }

        void wait() {
            std::unique_lock<std::mutex> lock(_inFlightMutex);
            _completed.wait(lock);
        }


        template<typename TJob, typename ...TArgs>
        un::DynamicChain<TJobSystem> enqueue(un::Dynamic<TJobSystem> dynamic, TArgs... args) {
            un::DynamicChain<TJobSystem> chain(this);
            chain.template enqueue<TJob>(dynamic, args...);
            return std::move(chain);
        }
    };

    template<typename TJobSystem>
    DynamicChain<TJobSystem>::DynamicChain(
        ptr<un::DynamicFlow<TJobSystem>> flow,
        bool dispatchOnDispose
    ) : un::JobChain<TJobSystem>(flow->_jobSystem, dispatchOnDispose),
        _flow(flow) { }

    template<typename TJobSystem>
    DynamicChain<TJobSystem>&& DynamicChain<TJobSystem>::dynamic(
        un::JobHandle handle,
        un::Dynamic<TJobSystem> dyn
    ) {
        _dynamics[handle] = dyn;
        return std::move(*this);
    }

    template<typename TJobSystem>
    DynamicChain<TJobSystem>::~DynamicChain() {
        if (this->dispatchOnDestruct) {
            _flow->include(*this, true);
        }
    }
}
#endif
