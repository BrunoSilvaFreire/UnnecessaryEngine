#include <unnecessary/systems/system.h>
#include <unnecessary/systems/simulation.h>

namespace un {
    Problem::Problem(const std::string& description) : description(description) {}

    const std::string& Problem::getDescription() const {
        return description;
    }


    const std::vector<Problem>& SystemDescriptor::getProblems() const {
        return problems;
    }

    void SystemDescriptor::runsAfterStage(const std::string& stageName) {
        u32 stageId;
        if (!simulation->findStage(stageName, &stageId)) {
            std::string msg = "SimpleSystem is trying to run after stage '";
            msg += stageName;
            msg += "' which doesn't exist.";
            problems.emplace_back(msg);
            return;
        }
        simulation->simulationGraph.addDependency(id, stageId);
    }

    void SystemDescriptor::runsOnStage(const std::string& stageName) {
        u32 stageId;
        if (!simulation->findStage(stageName, &stageId)) {
            std::string msg = "SimpleSystem is trying to be run on stage '";
            msg += stageName;
            msg += "' which doesn't exist.";
            problems.emplace_back(msg);
            return;
        }

        simulation->simulationGraph.addDependency(stageId, id);
    }

    SystemDescriptor::SystemDescriptor(
        u32 id,
        Simulation* simulation
    ) : id(id),
        simulation(simulation) {

    }

    void SimpleSystem::scheduleJobs(World& world, float deltaTime, JobChain& chain) {
        u32 scheduledId;
        chain.immediately<un::RunSystemJob>(&scheduledId, &world, this, deltaTime);
        std::string name;
        name += "Run system ";
        name += un::demangle(typeid(*this).name());
        chain.getJob(scheduledId)->setName(name);
    }
}
