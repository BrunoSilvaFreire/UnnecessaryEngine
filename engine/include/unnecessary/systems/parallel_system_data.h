
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
        un::RunSystemJob job;
        std::vector<un::System*> dependencies;
    public:
        ParallelSystemData(un::World* world, un::System* system) : job(world, system) {

        }

        un::System* getSystem() const {
            return job.getSystem();
        }

        un::System* getSystem() {
            return job.getSystem();
        }

        un::RunSystemJob& getJob();

        const un::RunSystemJob& getJob() const;

        const std::vector<un::System*>& getDependencies();

        void mustRunAfter(un::System* other);

        void mustRunAfter(un::ParallelSystemData& other);
    };
}
#endif
