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
    template<typename t_job_system>
    class job_chain {
    public:
        using job_system_type = t_job_system;
        using dispatch_table = typename job_system_type::dispatch_table;

    protected:
        job_system_type* _system;
        dispatch_table _toStart;
        dispatch_table _allJobs;
        bool _dispatchOnDestruct;

    public:
        explicit job_chain(
            job_system_type* system,
            bool dispatchOnDestruct = true
        ) : _system(system),
            _dispatchOnDestruct(dispatchOnDestruct) {
        }

        virtual ~job_chain() {
            if (_dispatchOnDestruct) {
                dispatch();
            }
        }

        job_chain(const job_chain<t_job_system>&) = delete;

        job_chain(job_chain<t_job_system>&& other) noexcept
            : _system(std::move(other._system)),
              _toStart(std::move(other._toStart)),
              _allJobs(std::move(other._allJobs)),
              _dispatchOnDestruct(other._dispatchOnDestruct) {
            other._dispatchOnDestruct = false;
        };

        void set_dispatch_on_destruct(bool dispatchOnDestruct) {
            _dispatchOnDestruct = dispatchOnDestruct;
        }

        template<typename TJob>
        job_chain& after(job_handle afterThis, job_handle runThis) {
            _system->add_dependency(afterThis, runThis);
            _toStart.template erase<typename TJob::worker_type>(runThis);
            return *this;
        }

        template<typename t_archetype>
        job_chain& after_all(job_handle runThis) {
            for (job_handle handle : _allJobs.template get_batch<t_archetype>()) {
                if (handle != runThis) {
                    after<typename t_archetype::job_type>(handle, runThis);
                }
            }
            return *this;
        }

        template<typename worker_type>
        job_chain& immediately(job_handle id) {
            _toStart.template dispatch<worker_type>(id);
            _allJobs.template dispatch<worker_type>(id);
            return *this;
        }

        template<typename T, typename... Args>
        job_chain& after(
            const std::initializer_list<job_handle>& dependencies,
            Args... args
        ) {
            auto job = new T(std::forward<Args>(args)...);
            auto id = _system->enqueue(job);
            for (job_handle dependency : dependencies) {
                _system->add_dependency(dependency, id);
            }
            _allJobs.emplace(id);
            return *this;
        }

        template<typename TJob>
        job_chain& after(job_handle afterThis, job_handle* runThis, TJob* job) {
            immediately<TJob>(runThis, job);
            job_handle handle = *runThis;
            after<TJob>(afterThis, handle);
            return *this;
        }

        template<typename J>
        job_chain& immediately(J* job) {
            std::pair<
                job_handle,
                job_node*
            > pair = _system->template create<typename J::worker_type>(
                job,
                false
            );
            return immediately<typename J::worker_type>(pair.second->poolLocalIndex);
        }

        template<typename t_job>
        job_chain& immediately(job_handle* id, t_job* job) {
            using worker_type = typename t_job::worker_type;
            std::pair<job_handle, job_node*> pair = _system->template create<worker_type>(
                job,
                false
            );
            job_handle handle = *id = pair.second->poolLocalIndex;
            immediately<worker_type>(handle);
            return *this;
        }

        template<typename TJob, typename... Args>
        job_chain& immediately(Args... args) {
            return immediately<TJob>(new TJob(std::forward<Args>(args)...));
        }

        template<typename TJob, typename... Args>
        job_chain& immediately(job_handle* id, Args... args) {
            return immediately<TJob>(id, new TJob(std::forward<Args>(args)...));
        }

        template<typename TJob>
        job_chain& after(job_handle dependencyId, TJob* job) {
            job_handle handle;
            immediately(&handle, job);
            after<TJob>(dependencyId, handle);
            return *this;
        }

        template<typename J, typename... Args>
        job_chain& after(job_handle afterThis, Args... args) {
            after<J>(afterThis, new J(std::forward<Args>(args)...));
            return *this;
        }

        template<typename J, typename... Args>
        job_chain& after(job_handle afterThis, job_handle* runThis, Args... args) {
            after<J>(afterThis, runThis, new J(std::forward<Args>(args)...));
            return *this;
        }

        void dispatch() {
            _system->dispatch(_toStart);
        }

        job_system_type* get_system() const {
            return _system;
        }

        template<typename Worker = job_worker, typename Lambda>
        void finally(Lambda callback) {
            job_handle job;
            immediately<lambda_job<Worker>>(&job, callback);
            job_system_type::for_each_archetype(
                [this, job]<typename t_archetype, size_t Index>() {
                    after_all<t_archetype>(job);
                }
            );
        }

        void set_name(job_handle i, const std::string& name) {
            _system->set_name(i, name);
        }

        dispatch_table get_all_jobs() const {
            return _allJobs;
        }

        std::size_t get_num_jobs() {
            std::size_t n = 0;
            job_system_type::for_each_archetype(
                [&n, this]<typename archetype, size_t Index>() {
                    n += _allJobs.template get_batch<archetype>().size();
                }
            );
            return n;
        }
    };
}
#endif
