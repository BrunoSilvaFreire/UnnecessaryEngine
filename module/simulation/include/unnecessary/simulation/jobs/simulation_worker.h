#ifndef UNNECESSARYENGINE_SIMULATION_WORKER_H
#define UNNECESSARYENGINE_SIMULATION_WORKER_H

#include <unnecessary/jobs/workers/worker.h>

namespace un {
    class SimulationWorker;

    typedef un::Job<un::SimulationWorker> SimulationJob;

    class SimulationWorker : public un::AbstractJobWorker<un::SimulationJob> {
    private:
    public:
        SimulationWorker(
            size_t index,
            bool autostart
        );
    };
}
#endif
