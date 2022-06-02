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

        WorkerLogger(WorkerType* type) : _worker(type) {
            using JobType = typename WorkerType::JobType;
            type->onExited() += [this]() {
                LOG(INFO) << worker_header() << " exited.";
            };
            type->onSleeping() += [this]() {
                LOG(INFO) << worker_header() << " is sleeping.";
            };
            type->onAwaken() += [this]() {
                LOG(INFO) << worker_header() << " has awaken.";
            };
            type->onStarted() += [this]() {
                LOG(INFO) << worker_header() << " has started.";
            };
            type->onFetched() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " fetched job " << handle << " (" << job->getName() << ").";
            };
            type->onExecuted() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " executed job " << handle << " (" << job->getName() << ").";
            };
            type->onEnqueued() += [this](JobType* job, un::JobHandle handle) {
                LOG(INFO) << worker_header() << " was enqueued job " << handle << " (" << job->getName() << ").";
            };
        }
    };

    template<typename TWorker>
    class LoggerPool {
    private:
        std::vector<WorkerLogger<TWorker>> loggers;
    public:
        void addLogger(TWorker* worker) {
            loggers.emplace_back(worker);
        }
    };

    template<typename TJobSystem>
    class JobSystemLogger : public TJobSystem::ExtensionType {
    private:
        typename TJobSystem::template ArchetypeTuple<LoggerPool> loggers;
    public:
        void apply(typename TJobSystem::ExtensionType::TargetType& jobSystem) override {
            jobSystem.for_each_worker(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>(TArchetype* worker) {
                    std::get<TArchetypeIndex>(loggers).addLogger(worker);
                }
            );
        }
    };
}
#endif
