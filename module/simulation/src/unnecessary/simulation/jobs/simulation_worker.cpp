#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {

    SimulationWorker::SimulationWorker(
        size_t index,
        bool autostart,
        const JobProvider<JobType>& provider,
        const JobNotifier<JobType>& notifier
    ) : AbstractJobWorker(index, autostart, provider, notifier) {}
}