#include <unnecessary/systems/simulation.h>

#include <utility>
#include <unnecessary/systems/system.h>

namespace un {

    SimulationNode::SimulationNode(
        std::string name
    ) : name(std::move(name)),
        type(un::SimulationNode::Type::eStage) {

    }

    SimulationNode::SimulationNode(
        std::string name,
        un::System* const system
    ) : name(std::move(name)),
        asSystem(system),
        type(un::SimulationNode::Type::eSystem) {

    }

    un::SimulationNode& SimulationNode::operator=(const SimulationNode& other) {
        name = other.name;
        type = other.type;
        switch (other.type) {
            case eSystem:
                asSystem = other.asSystem;
                break;
            case eStage:
                break;
        }
        return *this;
    }

    void Simulation::step(World& world, float deltaTime, un::JobChain& chain) {
        std::unordered_map<u32, std::set<u32>> systemToJobs;
        simulationGraph.each(
            [&](u32 index, const un::SimulationNode& group) {
                if (group.type != un::SimulationNode::Type::eSystem) {
                    return;
                }
                group.asSystem->scheduleJobs(world, deltaTime, chain);
                std::set<u32> scheduledJobs = chain.getRecentlyAdded();
                std::string jobsNames;
                for (const auto& item : scheduledJobs) {
                    jobsNames += std::to_string(item);
                    jobsNames += ", ";

                }
                LOG(INFO) << "Scheduled jobs" << jobsNames << " for " << group.name
                          << " ("
                          << index << ").";
                systemToJobs[index] = scheduledJobs;

                chain.separate();
            }
        );

        simulationGraph.each(
            [&](u32 index, const un::SimulationNode& node) {
                if (node.type != un::SimulationNode::Type::eSystem) {
                    return;
                }
                auto view = simulationGraph.allConnectionsOf(index);

                for (auto[neighbor, dependency] : view) {
                    auto dependencyType = simulationGraph.dependency(index, neighbor);
                    switch (dependencyType) {
                        case eUses:
                            runJobsAfter(systemToJobs, index, neighbor, chain);
                            break;
                        case eProvides:
                            // We run on this stage, gotta add dependencies of it
                            if (dependency->type != un::SimulationNode::Type::eStage) {
                                continue;
                            }
                            for (auto[otherDependencyIndex, otherDependency] : simulationGraph
                                .dependenciesOf(
                                    neighbor
                                )) {
                                if (otherDependency->type !=
                                    un::SimulationNode::Type::eStage) {
                                    continue;
                                }
                                for (auto[cousin, cousinType] : simulationGraph.dependenciesOf(
                                    otherDependencyIndex
                                )) {
                                    if (cousinType->type !=
                                        un::SimulationNode::Type::eSystem) {
                                        continue;
                                    }
                                    runJobsAfter(
                                        systemToJobs,
                                        index,
                                        cousin,
                                        chain
                                    );
                                }

                            }
                            break;
                    }
                }
            }
        );

    }

    bool Simulation::findStage(const std::string& stageName, u32* pInt) {
        auto it = stage2Id.find(stageName);
        if (it != stage2Id.end()) {
            *pInt = it->second;
            return true;
        }
        return false;
    }

    bool Simulation::tryDescribe(u32 index, un::System* system) {
        un::SystemDescriptor descriptor(index, this);
        system->describe(descriptor);
        const auto& problems = descriptor.getProblems();
        if (!problems.empty()) {
            LOG(FUCK)
                << "Unable to add system to simulation because of the following problems:";
            for (const un::Problem& problem : problems) {
                LOG(FUCK) << problem.getDescription();
            }
            simulationGraph.remove(index);
            return false;
        }
        return true;
    }

    void Simulation::addDependency(u32 from, u32 to) {
        simulationGraph.addDependency(from, to);
    }

    const DependencyGraph<un::SimulationNode>& Simulation::getSimulationGraph() const {
        return simulationGraph;
    }

    void Simulation::runJobsAfter(
        const std::unordered_map<u32, std::set<u32>>& map,
        u32 index,
        u32 neighbor,
        JobChain& chain
    ) {
        for (u32 from : map.at(index)) {
            for (u32 to : map.at(neighbor)) {
                if (from == to) {
                    continue;
                }
                chain.after(from, to);
            }
        }
    }

    template<>
    std::string un::to_string(const SimulationNode& node) {
        return node.name;
    }

    std::string SimulationGraph::getProperties(const SimulationNode* vertexType) const {
        switch (vertexType->type) {
            case un::SimulationNode::Type::eSystem:
                return "shape=box";
            case un::SimulationNode::Type::eStage:
                return "shape=circle";
        }
        return "";
    }
}