#include <unnecessary/systems/parallel_system_data.h>

namespace un {

    ParallelSystemData::ParallelSystemData(
            un::System *system,
            un::World *world
    ) : system(system), job(world, system) {}

    un::System *ParallelSystemData::getSystem() const {
        return system;
    }

    un::RunSystemJob &ParallelSystemData::getJob() {
        return job;
    }

    void ParallelSystemData::mustRunAfter(un::System *other) {
        dependencies.push_back(other);
    }

    const std::vector<un::System *> &ParallelSystemData::getDependencies() {
        return dependencies;
    }

    void ParallelSystemData::mustRunAfter(ParallelSystemData &other) {
        mustRunAfter(other.getSystem());
    }
}