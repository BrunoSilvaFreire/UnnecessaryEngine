#include <unnecessary/systems/system.h>
#include <unnecessary/systems/world.h>

namespace un {
    Problem::Problem(const std::string& description) : description(description) {}

    const std::string& Problem::getDescription() const {
        return description;
    }

    SystemDescriptor::SystemDescriptor(World* world) : world(world) {
        auto& systems = world->getSystems();
        for (auto[vertex, index] : systems.all_vertices()) {
            existingSystems.emplace_back(vertex->getSystem(), index);
        }
    }

    const std::vector<Problem>& SystemDescriptor::getProblems() const {
        return problems;
    }
}
