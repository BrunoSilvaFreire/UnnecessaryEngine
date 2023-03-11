//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
#define UNNECESSARYENGINE_PARALLEL_FOR_JOB_H

#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/misc/benchmark.h>

namespace un {
    template<typename t_job, typename t_worker>
    class parallelize_job : public job<t_worker> {
    private:
        t_job* _parallelForJob;
        size_t _fromIndex, _toIndex;

    public:
        parallelize_job(
            t_job* parallelForJob,
            size_t fromIndex,
            size_t toIndex
        ) : _parallelForJob(parallelForJob),
            _fromIndex(fromIndex),
            _toIndex(toIndex) {
            std::stringstream str;
            str << "Parallelize " << parallelForJob->get_name();
            str << "(" << fromIndex << " -> " << toIndex << ")";
            job<t_worker>::_name = str.str();
        }

        void operator()(t_worker* worker) override {
            _parallelForJob->batch_started(_fromIndex, _toIndex);
            for (size_t i = _fromIndex; i < _toIndex; ++i) {
                _parallelForJob->operator()(i, worker);
            }
            _parallelForJob->batch_finished(_fromIndex, _toIndex);
        }
    };

    template<typename worker_type>
    class parallel_for_job {
        std::string _name = "Unnamed Job";

    public:
        parallel_for_job() {
            _name = type_name_of(typeid(this));
        }

        const std::string& get_name() const {
            return _name;
        }

        void set_name(const std::string& newName) {
            _name = newName;
        }

        /**
         * Called before a batch is processed by a parallelize job
         * @param start The batch start
         * @param end The batch end
         */
        virtual void batch_started(size_t start, size_t end) {
        };

        virtual void batch_finished(size_t start, size_t end) {
        };

        virtual void operator()(size_t index, worker_type* worker) = 0;

        template<typename t_job, typename t_chain>
        static std::vector<job_handle> parallelize(
            t_job* job,
            t_chain& chain,
            size_t numEntries,
            size_t minNumberLoopsPerThread
        ) {
            using job_system_type = typename t_chain::JobSystemType;
            job_system_type* system = chain.get_system();
            auto& workerPool = system->template get_worker_pool<worker_type>();
            size_t numWorkers = workerPool.get_num_workers();
            size_t numEntriesPerJob = numEntries / numWorkers;
            if (numEntriesPerJob < minNumberLoopsPerThread) {
                numEntriesPerJob = minNumberLoopsPerThread;
            }
            size_t numFullJobs = numEntries / numEntriesPerJob;
            size_t totalFullyProcessedLoops = numFullJobs * numEntriesPerJob;
            size_t rest = numEntries - totalFullyProcessedLoops;
            using parallelizer = parallelize_job<t_job, worker_type>;
            std::vector<job_handle> handles;
            for (size_t i = 0; i < numFullJobs; ++i) {
                size_t from = i * numEntriesPerJob;
                size_t to = (i + 1) * numEntriesPerJob;
                job_handle handle;
                chain.template immediately<parallelizer>(&handle, job, from, to);
                handles.emplace_back(handle);
            }
            if (rest > 0) {
                job_handle handle;
                chain.template immediately<parallelizer>(
                    &handle,
                    job,
                    numEntries - rest,
                    numEntries
                );
                handles.emplace_back(handle);
            }
            return handles;
        }
    };
}
#endif //UNNECESSARYENGINE_PARALLEL_FOR_JOB_H
