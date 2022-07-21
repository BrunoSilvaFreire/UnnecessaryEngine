#ifndef UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H

#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/logger/job_system_logger.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>

namespace un {
    template<typename TJobSystem>
    class JobSystemBuilder {
    public:
        using WorkerAllocationConfig = typename TJobSystem::WorkerAllocationConfig;
    private:
        WorkerAllocationConfig allocationConfig;
        std::vector<std::shared_ptr<typename TJobSystem::ExtensionType>> extensions;
        bool autoStart;

        std::unique_ptr<TJobSystem> create() const {
            return std::make_unique<TJobSystem>(allocationConfig);
        }

    public:
        JobSystemBuilder() : allocationConfig(), autoStart(true) { };

        void setAutoStart(bool autoStart) {
            JobSystemBuilder::autoStart = autoStart;
        }

        template<typename TExtension, typename... Args>
        void withExtension(Args... args) {
            extensions.emplace_back(std::make_shared<TExtension>(args...));
        }

        void withRecorder() {
            withExtension<un::JobSystemRecorder<TJobSystem>>();
        }

        void withLogger() {
            withExtension<un::JobSystemLogger<TJobSystem>>();
        }

        template<typename TWorker>
        void setNumWorkers(std::size_t numWorkers) {
            constexpr auto WorkerIndex = TJobSystem::template index_of_archetype<TWorker>();
            std::get<WorkerIndex>(allocationConfig) = WorkerPoolConfiguration<TWorker>::forwarding(numWorkers);
        }

        template<typename TWorker>
        void setNumWorkers(float percentageOfAvailableThreads, u32 limit) {
            float percentage = static_cast<f32>(std::thread::hardware_concurrency()) * percentageOfAvailableThreads;
            u32 candidate = std::min(static_cast<u32>(std::floor(percentage)), limit);
            setNumWorkers<TWorker>(candidate);
        }

        template<typename TWorker>
        void fillWorkers() {
            setNumWorkers<TWorker>(std::thread::hardware_concurrency());
        }

        template<typename TWorker>
        std::size_t getNumWorkers() const {
            constexpr auto WorkerIndex = TJobSystem::template index_of_archetype<TWorker>();
            return std::get<WorkerIndex>(allocationConfig).getNumWorkers();
        }

        std::unique_ptr<TJobSystem> build() const {
            std::unique_ptr<TJobSystem> system = create();
            for (const auto& extension : extensions) {
                system->extendWith(extension);
            }
            if (autoStart) {
                system->start();
            }
            return system;
        }

    };

    template<>
    JobSystemBuilder<un::SimpleJobSystem>::JobSystemBuilder();

}
#endif
