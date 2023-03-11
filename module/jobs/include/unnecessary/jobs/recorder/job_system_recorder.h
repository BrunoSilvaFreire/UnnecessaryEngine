#ifndef UNNECESSARYENGINE_JOB_SYSTEM_RECORDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_RECORDER_H

#include <vector>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/recorder/worker_recorder.h>
#include <ostream>

namespace un {
    template<typename worker_type>
    class profiler_pool {
    private:
        std::vector<worker_recorder<worker_type>*> _profilers;

    public:
        void bootstrap(worker_pool<worker_type>* fromPool) {
            for (worker_type* item : fromPool->get_workers()) {
                _profilers.emplace_back(new worker_recorder<worker_type>(item));
            }
        }

        const std::vector<worker_recorder<worker_type>*>& get_profilers() const {
            return _profilers;
        }

        virtual ~profiler_pool() {
            for (const auto& item : _profilers) {
                delete item;
            }
        }
    };

    template<typename t_job_system>
    class job_system_recorder : public t_job_system::extension_type {
    private:
        using profiler_pool_tuple = typename t_job_system::profiler_pool_tuple;
        profiler_pool_tuple _tuple;

    public:
        job_system_recorder() : _tuple() {
        }

        virtual ~job_system_recorder() = default;

        void apply(typename t_job_system::extension_type::target_type& jobSystem) override {
            t_job_system::for_each_archetype(
                [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                    profiler_pool<t_archetype>& profilerPool = std::get<TArchetypeIndex>(_tuple);
                    worker_pool<t_archetype>& workerPool = jobSystem.template get_worker_pool<
                        t_archetype
                    >();
                    profilerPool.bootstrap(&workerPool);
                }
            );
        }

        std::string to_csv() const {
            std::stringstream os;
            os << "archetype,worker,handle,label,time,type" << std::endl;
            t_job_system::for_each_archetype(
                [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                    const auto& profilerPool = std::get<TArchetypeIndex>(_tuple);
                    for (const auto& item : profilerPool.get_profilers()) {
                        const event_history& history = item->get_history();
                        u32 j = 0;
                        for (const auto& event : history.get_events()) {
                            std::chrono::time_point timePoint = event.get_time();
                            const auto& count = timePoint.time_since_epoch().count();
                            os << TArchetypeIndex << ","
                               << item->get_worker()->get_index() << ",";
                            job_handle handle;
                            const std::unique_ptr<event_meta>& meta = event.get_meta();
                            if (meta->try_get_handle(handle)) {
                                os << handle << ",";
                            }
                            else {
                                os << ",";
                            }
                            os
                                << "\"" << meta->get_label() << "\","
                                << count << ","
                                << meta->get_event_type()
                                << std::endl;
                        }
                    }
                }
            );
            return os.str();
        }

        void save_to_file(std::filesystem::path file) {
            std::ofstream stream(file);
            stream << to_csv();
            stream.close();
        }
    };
}
#endif
