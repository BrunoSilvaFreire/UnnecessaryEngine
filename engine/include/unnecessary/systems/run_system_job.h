#ifndef UNNECESSARYENGINE_RUN_SYSTEM_JOB_H
#define UNNECESSARYENGINE_RUN_SYSTEM_JOB_H

#include <unnecessary/jobs/jobs.h>
#include <unnecessary/systems/system.h>

namespace un {
    class RunSystemJob : public Job {
    private:
        un::World *world;
        un::System *system;
        f32 deltaTime;
    public:
        void setDeltaTime(f32 deltaTime);

        RunSystemJob(un::World *world, un::System *system);

        explicit operator void() override;
    };
}
#endif
