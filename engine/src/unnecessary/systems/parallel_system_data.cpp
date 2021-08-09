#include <unnecessary/systems/parallel_system_data.h>
#include <unnecessary/systems/run_system_job.h>

namespace un {

    ParallelSystemData::ParallelSystemData(
            un::System *system,
            un::World *world
    ) : job(world, system) {}

    un::System *ParallelSystemData::getSystem() const {
        return job.getSystem();
    }

    un::RunSystemJob &ParallelSystemData::getJob() {
        return job;
    }

    void ParallelSystemData::mustRunAfter(un::System *other) {
        dependencies.push_back(other);
    }

    const un::RunSystemJob &ParallelSystemData::getJob() const {
        return job;
    }

    const std::vector<un::System *> &ParallelSystemData::getDependencies() {
        return dependencies;
    }

    void ParallelSystemData::mustRunAfter(ParallelSystemData &other) {
        mustRunAfter(other.getSystem());
    }
}