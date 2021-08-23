#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/def.h>
#include <unnecessary/jobs/jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/systems/world.h>

namespace un {

    struct System;

    class Problem {
    private:
        std::string description;
    public:
        Problem(const std::string& description);

        const std::string& getDescription() const;
    };


    class SystemDescriptor {
    private:
        std::vector<std::pair<un::System*, u32>> existingSystems;
        u32 id;
        std::vector<Problem> problems;
        un::World* world;
    public:
        SystemDescriptor(World* world);

        template<typename T>
        T* dependsOn();

        const std::vector<Problem>& getProblems() const;

        template<typename T>
        T* renderer(un::Renderer* renderer) {
            auto pipeline = renderer->getCurrentPipeline();
            if (pipeline == nullptr) {
                problems.emplace_back(
                    "There is no rendering pipeline created in the renderer."
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

    template<typename T>
    T* SystemDescriptor::dependsOn() {
        for (auto[system, other] :existingSystems) {
            T* candidate = dynamic_cast<T*>(system);
            if (candidate != nullptr) {
                world->systemMustRunAfter(id, other);
                return candidate;
            }
        }

        std::string str = "Missing system dependency of type '";
        str += typeid(T).name();
        str += "'.";
        problems.emplace_back(str);
        return nullptr;
    }

    struct ExplicitSystem {
        virtual void scheduleJobs(World& world, float deltaTime, JobChain& chain) = 0;

        virtual void describe(un::SystemDescriptor& descriptor) {}
    };

    struct System : public ExplicitSystem {
    public:
        void scheduleJobs(World& world, float deltaTime, JobChain& chain) override {
            chain.immediately<LambdaJob>(
                [this, &world, deltaTime](un::JobWorker* worker) {
                    step(world, deltaTime, worker);
                }
            );
        }

        virtual void step(World& world, f32 delta, un::JobWorker* worker) = 0;
    };

}
#endif
