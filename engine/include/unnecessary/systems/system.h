#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/def.h>
#include <unnecessary/jobs/jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/systems/run_system_job.h>

namespace un {

    struct System;

    class Problem {
    private:
        std::string description;
    public:
        Problem(const std::string& description);

        const std::string& getDescription() const;
    };

    class Simulation;

    class SystemDescriptor {
    private:
        u32 id;
        un::Simulation* simulation;
        std::vector<Problem> problems;
    public:
        SystemDescriptor(u32 id, Simulation* simulation);

        template<typename T>
        T* dependsOn() {

            for (auto[system, other] : simulation->system2Id) {
                T* candidate = dynamic_cast<T*>(system);
                if (candidate != nullptr) {
                    simulation->simulationGraph.addDependency(id, other);
                    return candidate;
                }
            }

            std::string str = "Missing system dependency of type '";
            str += typeid(T).name();
            str += "'.";
            problems.emplace_back(str);
            return nullptr;
        }

        void runsAfterStage(const std::string& stageName);

        void runsOnStage(const std::string& stageName);

        const std::vector<Problem>& getProblems() const;

        template<typename T>
        T* usesPipeline(un::Renderer* renderer) {
            auto pipeline = renderer->getCurrentPipeline();
            if (pipeline == nullptr) {
                problems.emplace_back(
                    "There is no rendering pipeline created in the usesPipeline."
                );
            }
            T* cast = dynamic_cast<T*>(pipeline);
            if (cast == nullptr) {
                std::string msg = "Current rending pipeline is not of type '";
                msg += typeid(T).name();
                msg += "'.";
                problems.emplace_back(msg);
                return nullptr;
            }
            return cast;
        }
    };

    struct ExplicitSystem {
        virtual void scheduleJobs(World& world, float deltaTime, JobChain& chain) = 0;

        virtual void describe(un::SystemDescriptor& descriptor) {}
    };

    struct System : public ExplicitSystem {
    public:
        void scheduleJobs(World& world, float deltaTime, JobChain& chain) override;

        virtual void step(World& world, f32 delta, un::JobWorker* worker) = 0;
    };


}
#endif
