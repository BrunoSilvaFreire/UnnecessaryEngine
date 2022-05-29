#ifndef UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_BUILDER_H

#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>

namespace un {
    template<typename TJobSystem>
    class JobSystemBuilder;

    template<typename TJobSystem>
    class JobSystemPackage {
    private:

        std::unique_ptr<TJobSystem> jobSystem;
        std::optional<un::JobSystemRecorder<TJobSystem>> recorder;
    public:
        TJobSystem& getJobSystem() {
            return *jobSystem;
        }

        explicit JobSystemPackage(std::unique_ptr<TJobSystem>&& jobSystem) : jobSystem(std::move(jobSystem)) { }

        JobSystemPackage(JobSystemPackage&& moved) :
            jobSystem(std::move(moved.jobSystem)),
            recorder(std::move(moved.recorder)) {

        }

        void addRecorder() {
            recorder = un::JobSystemRecorder<TJobSystem>(jobSystem.get());
        }

        friend class JobSystemBuilder<TJobSystem>;
    };

    template<typename TJobSystem>
    class JobSystemBuilder {
    public:
        using WorkerAllocationConfig = typename TJobSystem::WorkerAllocationConfig;
    private:
        WorkerAllocationConfig allocationConfig;
    public:
        template<typename TWorker>
        void setNumWorkers(std::size_t numWorkers) {
            constexpr auto WorkerIndex = TJobSystem::template index_of_archetype<TWorker>();
            std::get<WorkerIndex>(allocationConfig) = WorkerPoolConfiguration<TWorker>::forwarding(numWorkers);
        }

        template<typename TWorker>
        std::size_t getNumWorkers() const {
            constexpr auto WorkerIndex = TJobSystem::template index_of_archetype<TWorker>();
            return std::get<WorkerIndex>(allocationConfig).getNumWorkers();
        }

        TJobSystem build() const {
            return TJobSystem(allocationConfig);
        }

    };

    template<>
    class JobSystemBuilder<un::SimpleJobSystem> {
    private:
        bool autoStart = true;
        bool addRecorder = false;
        std::size_t numWorkers = std::thread::hardware_concurrency();
    public:
        void withRecorder() {
            addRecorder = true;
        }

        void setAutoStart(bool autoStart) {
            JobSystemBuilder::autoStart = autoStart;
        }

        void setNumWorkers(size_t numWorkers) {
            JobSystemBuilder::numWorkers = numWorkers;
        }

        JobSystemPackage<un::SimpleJobSystem>&& build() const {
            bool start = !addRecorder && autoStart;
            JobSystemPackage<un::SimpleJobSystem> package(
                std::move(
                    std::make_unique<un::SimpleJobSystem>(
                        numWorkers,
                        start
                    )));
            if (addRecorder) {
                package.addRecorder();
                if (autoStart) {
                    package.getJobSystem().start();
                }
            }
            return std::move(package);
        }
    };


}
#endif
