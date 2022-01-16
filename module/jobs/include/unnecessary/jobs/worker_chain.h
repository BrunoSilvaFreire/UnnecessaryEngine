//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_CHAIN_H
#define UNNECESSARYENGINE_WORKER_CHAIN_H

#include <concepts>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/misc/lambda_job.h>

namespace un {
    template<typename _Worker>
    class WorkerChain {
    public:
        typedef _Worker WorkerType;
        typedef typename WorkerType::JobType JobType;
    private:
        /**
         * A job that has been added to this worker chain.
         * Note that has not yet been submitted for execution
         * unless @see submit has been called.
         */
        class BookedJob {
        private:
            std::set<un::JobHandle> localDependencies;
            /**
             * The job pointer.
             */
            JobType* job;
        public:
            BookedJob(JobType* job) : job(job) {}

            friend class WorkerChain<WorkerType>;

        };

        /**
         * Jobs that have no dependencies
         */
        std::set<un::JobHandle> independent;
        /**
         * Jobs that no one depends on
         */
        std::set<un::JobHandle> leafs;
        std::vector<BookedJob> jobs;
    public:
        WorkerChain() : independent(), jobs() {}

        un::JobHandle immediately(JobType* job) {
            un::JobHandle handle = jobs.size();
            jobs.push_back(job);
            independent.emplace(handle);
            leafs.emplace(handle);
            return handle;
        }

        void after(un::JobHandle dependsOn, un::JobHandle job) {
            BookedJob booked = jobs[job];
            independent.erase(job);
            leafs.erase(dependsOn);
            booked.localDependencies.emplace(dependsOn);
        }

        un::JobHandle after(un::JobHandle dependsOn, JobType* job) {
            un::JobHandle handle = jobs.size();
            BookedJob booked = jobs.push_back(job);
            leafs.erase(dependsOn);
            booked.localDependencies.emplace(dependsOn);
            return handle;
        }

        template<typename T, typename ...Args>
        un::JobHandle immediately(Args ...args) {
            return immediately(new T(args...));
        }

        un::JobHandle finally(
            typename un::LambdaJob<WorkerType>::VoidCallback callback
        ) {
            un::JobHandle job = immediately<un::LambdaJob<WorkerType>>(callback);
            for (un::JobHandle leaf : leafs) {
                after(leaf, job);
            }
            return job;
        }

        void include(const un::WorkerChain<WorkerType>& other) {
            const std::vector<BookedJob>& otherJobs = other.jobs;
            std::vector<u32> newHandles(otherJobs.size());
            for (std::size_t i = 0; i < otherJobs.size(); ++i) {
                const auto& job = otherJobs[i];
                newHandles[i] = immediately(job.job);
            }
            for (std::size_t i = 0; i < otherJobs.size(); ++i) {
                un::JobHandle newHandle = newHandles[i];
                for (std::size_t j : otherJobs[i].localDependencies) {
                    un::JobHandle dependencyHandle = newHandles[j];
                    after(dependencyHandle, newHandle);
                }
            }
        }

        template<typename JobSystemType>
        void submit(JobSystemType* jobSystem) {
            std::vector<un::JobHandle> transformedHandles(jobs.size());
            for (std::size_t i = 0; i < jobs.size(); ++i) {
                transformedHandles[i] = jobSystem->enqueue(
                    jobs[i].job,
                    false
                );
            }
            std::set<un::JobHandle> handles;
            for (un::JobHandle starter : independent) {
                handles.emplace(transformedHandles[starter]);
            }
            jobSystem->template dispatch<WorkerType>(handles);
        }
    };
}
#endif
