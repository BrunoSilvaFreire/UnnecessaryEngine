#ifndef UNNECESSARYENGINE_SIMULATION_H
#define UNNECESSARYENGINE_SIMULATION_H

#include <vector>
#include <unnecessary/algorithm/dependency_graph.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/misc/types.h>

#define __JOB_GROUP_DEFINITION(name, definition) constexpr const char*const name = #definition
#define JOB_GROUP(x) __JOB_GROUP_DEFINITION(k##x, x);

namespace un {

    JOB_GROUP(UploadFrameData);
    JOB_GROUP(PrepareFrame);
    JOB_GROUP(RecordFrame);
    JOB_GROUP(DispatchFrame);
    JOB_GROUP(EarlyGameplay);
    JOB_GROUP(LateGameplay);

    class SimpleSystem;
    class ExplicitSystem;

    class World;

    class JobSystem;

    struct SimulationNode {
        enum Type {
            eSystem,
            eStage
        };
        std::string name;
        Type type;
        union {
            un::ExplicitSystem* asSystem;
            struct {
                // Reserved for future use
            } asStage;
        };

        explicit SimulationNode(std::string name);

        SimulationNode(std::string name, un::ExplicitSystem* system);

        un::SimulationNode& operator=(const un::SimulationNode& other);
    };

    template<>
    std::string to_string(const un::SimulationNode& node);

    class SimulationGraph : public un::DependencyGraph<un::SimulationNode> {
    protected:
        std::string getProperties(const SimulationNode* vertexType) const override;
    };

    class Simulation {
    private:
        un::SimulationGraph simulationGraph;
        std::unordered_map<ExplicitSystem*, u32> system2Id;
        std::unordered_map<std::string, u32> stage2Id;

        bool findStage(
            const std::string& stageName,
            u32* pInt
        );

        bool tryDescribe(u32 index, ExplicitSystem* system);
        void runJobsAfter(
            const std::unordered_map<u32, std::set<u32>>& map,
            u32 index,
            u32 neighbor,
            JobChain& chain
        );
    public:
        void addDependency(u32 from, u32 to);

        template<typename T, typename ...Args>
        std::pair<u32, T*> addSystem(Args... args) {
            T* system = new T(args...);
            u32 id = simulationGraph.add(un::type_name_of<T>(), system);
            if (!tryDescribe(id, system)) {
                return std::pair(-1, nullptr);
            }
            system2Id[system] = id;
            return std::pair(id, system);
        };

        u32 addStage(const std::string& name) {
            u32 id = simulationGraph.add(name);
            stage2Id[name] = id;
            return id;
        }

        void step(un::World& world, float deltaTime, un::JobChain& chain);

        const DependencyGraph<un::SimulationNode>& getSimulationGraph() const;

        friend class SystemDescriptor;
    };
}
#endif
