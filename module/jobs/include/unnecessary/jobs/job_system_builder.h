#ifndef UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H

#include <cmath>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/logger/job_system_logger.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>

namespace un {
    template<typename t_job_system>
    class job_system_builder {
    public:
        using job_system_type = t_job_system;
        using extension_type = typename job_system_type::extension_type;
        using worker_allocation_config = typename job_system_type::worker_allocation_config;

    private:
        worker_allocation_config _allocationConfig;
        std::vector<std::shared_ptr<extension_type>> _extensions;
        bool _autoStart;

        std::unique_ptr<t_job_system> create() const {
            return std::make_unique<t_job_system>(_allocationConfig);
        }

    public:
        job_system_builder() : _allocationConfig(), _autoStart(true) {
        };

        void set_auto_start(bool autoStart) {
            _autoStart = autoStart;
        }

        template<typename TExtension, typename... Args>
        void with_extension(Args... args) {
            _extensions.emplace_back(std::make_shared<TExtension>(args...));
        }

        void with_recorder() {
            with_extension<job_system_recorder<t_job_system>>();
        }

        void with_logger() {
            with_extension<job_system_logger<t_job_system>>();
        }

        template<typename worker_type>
        void set_num_workers(std::size_t numWorkers) {
            constexpr auto worker_index = job_system_type::template index_of_archetype<worker_type>();
            std::get<worker_index>(_allocationConfig) = worker_pool_configuration<worker_type>::forwarding(
                numWorkers
            );
        }

        template<typename t_worker>
        void set_num_workers(float percentageOfAvailableThreads, u32 limit) {
            float percentage = static_cast<f32>(std::thread::hardware_concurrency())
                               * percentageOfAvailableThreads;
            u32 candidate = std::min(static_cast<u32>(std::floor(percentage)), limit);
            set_num_workers<t_worker>(candidate);
        }

        template<typename worker_type>
        void fill_workers() {
            set_num_workers<worker_type>(std::thread::hardware_concurrency());
        }

        template<typename worker_type>
        std::size_t get_num_workers() const {
            constexpr auto worker_index = job_system_type::template index_of_archetype<worker_type>();
            return std::get<worker_index>(_allocationConfig).get_num_workers();
        }

        std::unique_ptr<t_job_system> build() const {
            std::unique_ptr<t_job_system> system = create();
            for (const auto& extension : _extensions) {
                system->extend_with(extension);
            }
            if (_autoStart) {
                system->start();
            }
            return system;
        }
    };

    template<>
    job_system_builder<simple_job_system>::job_system_builder();
}
#endif
