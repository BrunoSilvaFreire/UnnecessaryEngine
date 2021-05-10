
#ifndef UNNECESSARYENGINE_PARALLEL_SYSTEM_DATA_H
#define UNNECESSARYENGINE_PARALLEL_SYSTEM_DATA_H

#include <unnecessary/systems/run_system_job.h>

namespace un {

    class ParallelSystemData {
    private:
        un::System *system;
        un::RunSystemJob job;
    public:
        ParallelSystemData(un::System *system, un::World *world);

        un::System *getSystem() const;

        un::RunSystemJob &getJob();

        const std::vector<un::System *> &getDependencies() ;
    };
}
#endif
