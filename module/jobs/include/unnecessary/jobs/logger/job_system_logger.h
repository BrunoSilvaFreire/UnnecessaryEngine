#ifndef UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_LOGGER_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/logging.h>

namespace un {
    template<typename TWorker>
    class WorkerLogger {
    public:
        typedef TWorker WorkerType;
    private:
        WorkerType* _worker;

        std::string worker_header() {
            std::stringstream stream;
            stream << "Worker " << _worker->getIndex() << " (" << _worker->getName() << ")";
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
                LOG(INFO) << worker_header() << " exited.";
            };
            _worker->onSleeping() += [this]() {
                LOG(INFO) << worker_header() << " is sleeping.";
            };
            _worker->onAwaken() += [this]() {
                LOG(INFO) << worker_header() << " has awaken.";
            };
            _worker->onStarted() += [this]() {
                LOG(INFO) << worker_header() << " has started.";
            };
            _worker->onFetched() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " fetched job " << handle << " (" << job->getName() << ").";
            };
            _worker->onExecuted() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " executed job " << handle << " (" << job->getName() << ").";
            };
            _worker->onEnqueued() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " was enqueued job " << handle << " (" << job->getName() << ").";
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
                std::vector<std::string> dependenciesMissing;
                const un::DependencyGraph<un::JobNode>& jobGraph = jobSystem.getJobGraph();
                for (auto dependantIndex : jobGraph.dependenciesOf(handle)) {
                    auto dependantNode = jobGraph[dependantIndex];
                    TJobSystem::for_each_archetype(
                        [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                            if (dependantNode->archetypeIndex != TArchetypeIndex) {
                                return;
                            }
                            auto* job = jobSystem.template getWorkerPool<TArchetype>()
                                                 .getJob(dependantNode->poolLocalIndex);
                            std::string jobStr;
                            if (job == nullptr){
                                jobStr = std::string("Unknown Job (") + un::to_string(*dependantNode) +
                                         " of archetype " +
                                         un::type_name_of<TArchetype>() + ")";
                            } else {
                                jobStr = std::string("Job ") + job->getName() + " (" + un::to_string(*dependantNode) +
                                         " of archetype " +
                                         un::type_name_of<TArchetype>() + ")";
                            }

                            dependenciesMissing.push_back(jobStr);
                        }
                    );
                };
                TJobSystem::for_each_archetype(
                    [
                        this,
                        &node,
                        &jobSystem,
                        &dependenciesMissing
                    ]<typename TArchetype, std::size_t TArchetypeIndex>() {
                        if (node.archetypeIndex != TArchetypeIndex) {
                            return;
                        }

                        auto* job = jobSystem.template getWorkerPool<TArchetype>().getJob(node.poolLocalIndex);

                        LOG(INFO) << "Job \"" << job->getName() << "\" (" << un::type_name_of<TArchetype>()
                                  << ") failed to be unlocked because of dependencies (" << dependenciesMissing.size()
                                  << "): "
                                  << un::join_strings(dependenciesMissing.begin(), dependenciesMissing.end());
                    }
                );
            };
        }
    };
}
#endif
