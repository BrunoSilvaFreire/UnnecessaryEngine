#ifndef UNNECESSARYENGINE_RUN_SYSTEM_JOB_H
#define UNNECESSARYENGINE_RUN_SYSTEM_JOB_H

#include <unnecessary/jobs/jobs.h>

namespace un {
    class System;

    class World;

    class RunSystemJob : public Job {
    private:
        un::World* world;
        un::System* system;
        f32 deltaTime;
    public:
        un::System* getSystem() const {
            return system;
        }

        RunSystemJob(un::World* world, un::System* system, f32 deltaTime);

        void operator()(un::JobWorker* worker) override;
    };
}
#endif
