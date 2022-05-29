#include <unnecessary/jobs/simple_jobs.h>

namespace un {

    JobWorker::JobWorker(
        std::size_t index,
        bool autostart
    ) : AbstractJobWorker(index, autostart) { }

    SimpleJobSystem::SimpleJobSystem(size_t numWorkers, bool autoStart) : un::JobSystem<JobWorker>(
        un::WorkerPoolConfiguration<JobWorker>(
            numWorkers,
            [=](
                size_t index
            ) {
                return new JobWorker(index, autoStart);
            }
        )
    ) {

    }

    SimpleJobSystem::SimpleJobSystem(bool autoStart) : SimpleJobSystem(std::thread::hardware_concurrency(), autoStart) {

    }

    void SimpleJobSystem::start() {
        getWorkerPool<JobWorker>().start();
    }
}