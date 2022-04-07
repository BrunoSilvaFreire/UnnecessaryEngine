#ifndef UNNECESSARYENGINE_JOB_SYSTEM_RECORDER_H
#define UNNECESSARYENGINE_JOB_SYSTEM_RECORDER_H

#include <vector>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/recorder/worker_recorder.h>
#include <ostream>

namespace un {
    template<typename TWorker>
    class ProfilerPool {
    private:
        std::vector<un::WorkerRecorder<TWorker>*> profilers;
    public:
        void bootstrap(WorkerPool<TWorker>* fromPool) {
            for (TWorker* item : fromPool->getWorkers()) {
                profilers.emplace_back(new un::WorkerRecorder<TWorker>(item));
            }
        }

        const std::vector<un::WorkerRecorder<TWorker>*>& getProfilers() const {
            return profilers;
        }

        virtual ~ProfilerPool() {
            for (const auto& item : profilers) {
                delete item;
            }
        }
    };

    template<typename TJobSystem>
    class JobSystemRecorder {
    private:
        typedef typename TJobSystem::ProfilerPoolTuple ProfilerPoolTuple;
        ProfilerPoolTuple tuple;

    public:
        JobSystemRecorder(TJobSystem* jobSystem) : tuple() {
            TJobSystem::for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    ProfilerPool<TArchetype>& profilerPool = std::get<TArchetypeIndex>(
                        tuple
                    );
                    un::WorkerPool<TArchetype>& workerPool = jobSystem->template getWorkerPool<TArchetype>();
                    profilerPool.bootstrap(&workerPool);
                }
            );
        }

        virtual ~JobSystemRecorder() {

        }

        std::string toCSV() const {
            std::stringstream os;
            os << "archetype,worker,handle,label,time,type" << std::endl;
            TJobSystem::for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    const auto& profilerPool = std::get<TArchetypeIndex>(tuple);
                    for (const auto& item : profilerPool.getProfilers()) {
                        const un::EventHistory& history = item->getHistory();
                        u32 j = 0;
                        for (const auto& event : history.getEvents()) {

                            std::chrono::time_point timePoint = event.getTime();
                            const auto& count = timePoint.time_since_epoch().count();
                            os << TArchetypeIndex << ","
                               << item->getWorker()->getIndex() << ",";
                            un::JobHandle handle;
                            const std::unique_ptr<un::EventMeta>& meta = event.getMeta();
                            if (meta->tryGetHandle(handle)) {
                                os << handle << ",";
                            } else {
                                os << ",";
                            }
                            os
                                << "\"" << meta->getLabel() << "\","
                                << count << ","
                                << meta->getEventType()
                                << std::endl;
                        }
                    }
                }
            );
            return os.str();
        }

        void saveToFile(std::filesystem::path file) {
            std::ofstream stream(file);
            stream << toCSV();
            stream.close();
        }
    };
}
#endif