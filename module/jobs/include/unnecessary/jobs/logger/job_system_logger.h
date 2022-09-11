#ifndef UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/pretty_print.h>

namespace un {
    template<typename TWorker>
    class WorkerLogger {
    public:
        typedef TWorker WorkerType;
    private:
        WorkerType* _worker;

        std::string worker_header() {
            std::stringstream stream;
            stream << PURPLE("Worker " << _worker->getIndex()) << " (" << PURPLE(_worker->getName()) << ")";
            return stream.str();
        }

    public:

        WorkerType* getWorker() const {
            return _worker;
        }


        WorkerLogger(WorkerLogger&&) = delete;

        WorkerLogger(const WorkerLogger&) = delete;

        explicit WorkerLogger(WorkerType* type) : _worker(type) {
            using JobType = typename WorkerType::JobType;
            _worker->onExited() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << RED("exited") << ".";
            };
            _worker->onSleeping() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " is " << YELLOW("sleeping") << ".";
            };
            _worker->onAwaken() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " has " << GREEN("awaken") << ".";
            };
            _worker->onStarted() += [this]() {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " has " << GREEN("started") << " .";
            };
            _worker->onFetched() += [this](JobType* job, un::JobHandle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " " << YELLOW("fetched") << "  job " << GREEN(handle) << " ("
                               << GREEN(job->getName()) << ").";
            };
            _worker->onExecuted() += [this](JobType* job, un::JobHandle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " executed job " << GREEN(handle) << " (" << GREEN(job->getName())
                               << ").";
            };
            _worker->onEnqueued() += [this](JobType* job, un::JobHandle handle) {
                auto message = un::message("JobSystem");
                message.text() << worker_header() << " was enqueued job " << GREEN(handle) << " ("
                               << GREEN(job->getName())
                               << ").";
            };
        }
    };

    template<typename TWorker>
    class LoggerPool {
    private:
        std::vector<std::unique_ptr<WorkerLogger<TWorker>>> loggers;
    public:
        void addLogger(TWorker* worker) {
            loggers.emplace_back(std::make_unique<WorkerLogger<TWorker>>(worker));
        }
    };

    template<typename TJobSystem>
    class JobSystemLogger : public TJobSystem::ExtensionType {
    public:
        using LoggerPoolTuple = typename TJobSystem::template ArchetypeTuple<LoggerPool>;
    private:
        LoggerPoolTuple loggers;

    public:
        void apply(typename TJobSystem::ExtensionType::TargetType& jobSystem) override {
            jobSystem.for_each_worker(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>(TArchetype* worker) {
                    std::get<TArchetypeIndex>(loggers).addLogger(worker);
                }
            );
            jobSystem.onUnlockFailed() += [this, &jobSystem](un::JobHandle handle, const un::JobNode& node) {
                const un::DependencyGraph<un::JobNode>& jobGraph = jobSystem.getJobGraph();
                un::RichMessage root = un::message("JobSystem");
                auto& header = root.text();
                auto& missingJobsMsg = root.tree();
                for (auto dependantIndex : jobGraph.dependenciesOf(handle)) {
                    const un::JobNode* dependantNode = jobGraph[dependantIndex];
                    TJobSystem::for_each_archetype(
                        [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                            if (dependantNode->archetypeIndex != TArchetypeIndex) {
                                return;
                            }
                            auto* job = jobSystem.template getWorkerPool<TArchetype>()
                                                 .getJob(dependantNode->poolLocalIndex);
                            auto& tree = missingJobsMsg.map();
                            un::TextMessage& entry = tree.text("name");
                            if (job == nullptr) {
                                entry << "Unknown Job";
                            } else {
                                entry << job->getName();
                            }

                            tree.text("handle (poolLocalIndex)") << dependantNode->poolLocalIndex;
                            tree.text("archetype") << un::type_name_of<TArchetype>();
                            tree.text("archetypeIndex") << dependantNode->archetypeIndex;
                        }
                    );
                };
                TJobSystem::for_each_archetype(
                    [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                        if (node.archetypeIndex != TArchetypeIndex) {
                            return;
                        }

                        auto* job = jobSystem.template getWorkerPool<TArchetype>().getJob(node.poolLocalIndex);
                        header << "Job \"" << job->getName() << "\" (" << un::type_name_of<TArchetype>()
                               << ") failed to be unlocked because of dependencies ("
                               << missingJobsMsg.num_children() << ")";
                    }
                );
            };
        }
    };
}
#endif
