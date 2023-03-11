//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_CHAIN_H
#define UNNECESSARYENGINE_WORKER_CHAIN_H

#include <concepts>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/misc/lambda_job.h>

namespace un {
    template<typename t_worker>
    class worker_chain {
    public:
        using worker_type = t_worker;
        typedef typename worker_type::job_type job_type;
    private:
        /**
         * A job that has been added to this worker chain.
         * Note that has not yet been available for execution
         * unless @see submit has been called.
         */
        class booked_job {
        private:
            std::set<un::job_handle> _localDependencies;
            /**
             * The job pointer.
             */
            job_type* _job;
        public:
            booked_job(job_type* job) : _job(job) {
            }

            friend class worker_chain<worker_type>;

        };

        /**
         * Jobs that have no dependencies
         */
        std::set<un::job_handle> _independent;
        /**
         * Jobs that no one depends on
         */
        std::set<un::job_handle> _leafs;
        std::vector<booked_job> _jobs;
    public:
        worker_chain() : _independent(), _jobs() {
        }

        un::job_handle immediately(job_type* job) {
            un::job_handle handle = _jobs.size();
            _jobs.push_back(job);
            _independent.emplace(handle);
            _leafs.emplace(handle);
            return handle;
        }

        void after(un::job_handle dependsOn, un::job_handle job) {
            booked_job& booked = _jobs[job];
            _independent.erase(job);
            _leafs.erase(dependsOn);
            booked._localDependencies.emplace(dependsOn);
        }

        un::job_handle after(un::job_handle dependsOn, job_type* job) {
            un::job_handle handle = _jobs.size();
            _jobs.push_back(job);
            booked_job& booked = _jobs[handle];
            _leafs.erase(dependsOn);
            booked._localDependencies.emplace(dependsOn);
            return handle;
        }

        template<typename T, typename ...Args>
        un::job_handle immediately(Args ...args) {
            return immediately(new T(args...));
        }

        un::job_handle finally(
            typename un::lambda_job<worker_type>::void_callback callback
        ) {
            un::job_handle job = immediately<un::lambda_job<worker_type>>(callback);
            std::set<un::job_handle> existingLeafs = _leafs;
            for (un::job_handle leaf : existingLeafs) {
                if (leaf != job) {
                    after(leaf, job);
                }
            }
            return job;
        }

        un::job_handle finally(
            const std::string& name,
            typename un::lambda_job<worker_type>::void_callback callback
        ) {
            un::job_handle job = finally(callback);
            _jobs[job]._job->set_name(name);
            return job;
        }

        un::job_handle finally(
            typename un::lambda_job<worker_type>::callback callback
        ) {
            un::job_handle job = immediately<un::lambda_job<worker_type>>(callback);
            std::set<un::job_handle> existingLeafs = _leafs;
            for (un::job_handle leaf : existingLeafs) {
                after(leaf, job);
            }
            return job;
        }

        un::job_handle finally(
            const std::string& name,
            typename un::lambda_job<worker_type>::callback callback
        ) {
            un::job_handle job = finally(callback);
            _jobs[job]._job->set_name(name);
            return job;
        }

        void include(const un::worker_chain<worker_type>& other) {
            const std::vector<booked_job>& otherJobs = other._jobs;
            std::vector<u32> newHandles(otherJobs.size());
            for (std::size_t i = 0; i < otherJobs.size(); ++i) {
                const auto& job = otherJobs[i];
                newHandles[i] = immediately(job._job);
            }
            for (std::size_t i = 0; i < otherJobs.size(); ++i) {
                un::job_handle newHandle = newHandles[i];
                for (std::size_t j : otherJobs[i]._localDependencies) {
                    un::job_handle dependencyHandle = newHandles[j];
                    after(dependencyHandle, newHandle);
                }
            }
        }

        template<typename JobSystemType>
        void submit(JobSystemType* jobSystem) {
            {
                std::unique_lock<std::mutex> lock(jobSystem->_graphAccessMutex);
                constexpr std::size_t archetype_index = JobSystemType::template index_of_archetype<
                    worker_type
                >();
                un::worker_pool<worker_type>& pool = jobSystem->template get_worker_pool<worker_type>();
                std::vector<un::job_handle> transformedHandles(_jobs.size());
                for (std::size_t i = 0; i < _jobs.size(); ++i) {
                    un::job_handle graphHandle = jobSystem->_graph.add(
                        un::job_node{
                            .archetypeIndex = archetype_index
                        }
                    );
                    un::job_node* node = jobSystem->_graph[graphHandle];
                    auto transformedIndex = node->poolLocalIndex = pool.enqueue(
                        _jobs[i]._job,
                        graphHandle,
                        false
                    );
                    transformedHandles[i] = transformedIndex;
                }
                for (std::size_t i = 0; i < _jobs.size(); ++i) {
                    booked_job& bookedJob = _jobs[i];
                    job_handle jobHandle = transformedHandles[i];
                    for (job_handle localDependency : bookedJob._localDependencies) {
                        job_handle dependencyHandle = transformedHandles[localDependency];
                        jobSystem->_graph.add_dependency(jobHandle, dependencyHandle);
                    }

                }
                std::set<un::job_handle> handles;
                for (un::job_handle starter : _independent) {
                    handles.emplace(transformedHandles[starter]);
                }
                pool.dispatch(handles);
            }

        }

        void set_name(job_handle i, const std::string& name) {
            _jobs[i]._job->set_name(name);
        }
    };
}
#endif
