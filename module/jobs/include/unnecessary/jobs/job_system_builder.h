#ifndef UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H

#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>

namespace un {
    template<typename TJobSystem>
    class JobSystemBuilder {
    public:
        using WorkerAllocationConfig = typename TJobSystem::WorkerAllocationConfig;
    private:
        WorkerAllocationConfig allocationConfig;
        bool addRecorder;
        bool autoStart;

        std::unique_ptr<TJobSystem> create() const {
            return TJobSystem(allocationConfig, false);
        }

    public:
        JobSystemBuilder() : allocationConfig(), addRecorder(false), autoStart(true) { };

        void setAutoStart(bool autoStart) {
            JobSystemBuilder::autoStart = autoStart;
        }

        void withRecorder() {
            addRecorder = true;
        }

        template<typename TWorker>
        void setNumWorkers(std::size_t numWorkers) {
            constexpr auto WorkerIndex = TJobSystem::template index_of_archetype<TWorker>();
            std::get<WorkerIndex>(allocationConfig) = WorkerPoolConfiguration<TWorker>::forwarding(numWorkers);
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
            bool start = !addRecorder && autoStart;
            std::unique_ptr<TJobSystem> system = create();
            if (addRecorder) {
                system->extendWith(std::make_shared<un::JobSystemRecorder<TJobSystem>>());
                if (autoStart) {
                    system->start();
                }
            }
            return system;
        }

    };

    template<>
    std::unique_ptr<un::SimpleJobSystem> JobSystemBuilder<un::SimpleJobSystem>::create() const {
        return std::make_unique<un::SimpleJobSystem>(getNumWorkers<un::JobWorker>(), false);
    }

    template<>
    JobSystemBuilder<un::SimpleJobSystem>::JobSystemBuilder()
        : allocationConfig(), addRecorder(false), autoStart(true) {
        fillWorkers<un::JobWorker>();
    }

}
#endif
