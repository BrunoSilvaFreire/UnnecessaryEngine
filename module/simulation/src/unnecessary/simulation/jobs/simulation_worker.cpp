#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {

    SimulationWorker::SimulationWorker(
        size_t index,
        bool autostart
    ) : AbstractJobWorker(index, autostart) {}
}