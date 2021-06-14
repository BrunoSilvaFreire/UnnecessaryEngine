
#ifndef UNNECESSARYENGINE_PARALLEL_SYSTEM_DATA_H
#define UNNECESSARYENGINE_PARALLEL_SYSTEM_DATA_H
#include <vector>
#include <unnecessary/systems/run_system_job.h>
namespace un {
    class System;

    class RunSystemJob;

    class World;

    class ParallelSystemData {
    private:
        un::System *system;
        un::RunSystemJob job;
        std::vector<un::System *> dependencies;
    public:
        ParallelSystemData(un::System *system, un::World *world);

        un::System *getSystem() const;

        un::RunSystemJob &getJob();

        const un::RunSystemJob &getJob() const;

        const std::vector<un::System *> &getDependencies();

        void mustRunAfter(System *other);

        void mustRunAfter(ParallelSystemData &other);
    };
}
#endif
