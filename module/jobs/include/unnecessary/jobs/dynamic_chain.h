#ifndef UNNECESSARYENGINE_DYNAMIC_CHAIN_H
#define UNNECESSARYENGINE_DYNAMIC_CHAIN_H

#include <unordered_set>
#include <utility>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>

namespace un {

    template<typename t_job_system>
    class dynamic_flow;

    template<typename t_job_system>
    class dynamic_chain;

    template<typename t_job_system>
    using dynamic = std::function<void(un::dynamic_chain<t_job_system>& chain)>;

    template<typename t_job_system>
    class dynamic_chain : public un::job_chain<t_job_system> {
    private:
        un::ptr<un::dynamic_flow<t_job_system>> _flow;
        std::unordered_map<un::job_handle, un::dynamic<t_job_system>> _dynamics;
    public:
        explicit dynamic_chain(
            un::ptr<dynamic_flow<t_job_system>> flow,
            bool dispatchOnDispose = true
        );

        dynamic_chain(const un::dynamic_chain<t_job_system>&) = delete;

        dynamic_chain(un::dynamic_chain<t_job_system>&& other) noexcept
            : un::job_chain<t_job_system>(std::move(other)),
              _flow(std::move(other._flow)),
              _dynamics(std::move(other._dynamics)) {
            other._dispatchOnDestruct = false;

        };

        ~dynamic_chain() override;

        dynamic_chain&& dynamic(un::job_handle handle, dynamic<t_job_system> dyn);

        template<typename TJob, typename ...TArgs>
        un::dynamic_chain<t_job_system>& enqueue(un::dynamic<t_job_system> dyn, TArgs... args) {
            un::job_handle handle;
            this->template immediately<TJob>(&handle, std::forward<TArgs>(args)...);
            dynamic(handle, dyn);
            return *this;
        }

        template<typename TJob>
        un::dynamic_chain<t_job_system>& enqueue(un::dynamic<t_job_system> dyn, un::ptr<TJob> job) {
            un::job_handle handle;
            this->template immediately<TJob>(&handle, job);
            dynamic(handle, dyn);
            return *this;
        }

        friend class dynamic_flow<t_job_system>;
    };

    template<typename t_job_system>
    class dynamic_flow {
    public:
        friend class dynamic_chain<t_job_system>;
    private:
        un::ptr<t_job_system> _jobSystem;
        std::mutex _inFlightMutex;
        std::unordered_set<un::job_handle> _pendingJobs;
        std::unordered_map<un::job_handle, un::dynamic<t_job_system>> _dynamics;
        //
        std::condition_variable _completed;

        void include(un::dynamic_chain<t_job_system>& chain, bool lock) {
            if (lock) {
                _inFlightMutex.lock();
            }
            const auto& toAdd = chain._dynamics;
            _dynamics.insert(toAdd.begin(), toAdd.end());
            for (const auto& item : toAdd) {
                job_handle handle = item.first;
                _pendingJobs.insert(handle);
                job_handle dynHandle;
                chain.template after<lambda_job<>>(
                    handle,
                    &dynHandle,
                    [this, handle]() {
                        on_job_finished(handle);
                    }
                );
                std::stringstream ss;
                ss << "Invoke dynamic for job " << handle << ".";
                chain.set_name(dynHandle, ss.str());
            }

            if (lock) {
                _inFlightMutex.unlock();
            }
        }

        void on_job_finished(un::job_handle handle) {

            std::lock_guard<std::mutex> lock(_inFlightMutex);
            _pendingJobs.erase(handle);
            auto iterator = _dynamics.find(handle);
            if (iterator != _dynamics.end()) {
                auto& dynamic = iterator->second;
                un::dynamic_chain<t_job_system> subChain(this, false);
                dynamic(subChain);
                include(subChain, false);
                /* const auto& jobs = subChain.getAllJobs();
                 t_job_system::for_each_archetype(
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
        explicit dynamic_flow(un::ptr<t_job_system> jobSystem) : _pendingJobs(),
                                                                 _jobSystem(jobSystem) {
        }

        void wait() {
            std::unique_lock<std::mutex> lock(_inFlightMutex);
            _completed.wait(lock);
        }

        template<typename TJob, typename ...TArgs>
        un::dynamic_chain<t_job_system> enqueue(un::dynamic<t_job_system> dynamic, TArgs... args) {
            un::dynamic_chain<t_job_system> chain(this);
            chain.template enqueue<TJob>(dynamic, args...);
            return std::move(chain);
        }
    };

    template<typename t_job_system>
    dynamic_chain<t_job_system>::dynamic_chain(
        ptr<un::dynamic_flow<t_job_system>> flow,
        bool dispatchOnDispose
    ) : un::job_chain<t_job_system>(flow->_jobSystem, dispatchOnDispose),
        _flow(flow) {
    }

    template<typename t_job_system>
    dynamic_chain<t_job_system>&& dynamic_chain<t_job_system>::dynamic(
        un::job_handle handle,
        un::dynamic<t_job_system> dyn
    ) {
        _dynamics[handle] = dyn;
        return std::move(*this);
    }

    template<typename t_job_system>
    dynamic_chain<t_job_system>::~dynamic_chain() {
        if (this->_dispatchOnDestruct) {
            _flow->include(*this, true);
        }
    }
}
#endif
