

#ifndef UNNECESSARYENGINE_WORKER_H
#define UNNECESSARYENGINE_WORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include "unnecessary/jobs/job.h"

namespace un {

    template<class _Job>
    class AbstractJobWorker {
    public:
        typedef _Job JobType;
    private:
        size_t index;
        std::thread* thread;
        un::JobProvider<JobType> provider;
        un::JobNotifier<JobType> notifier;
        bool running = true;
        bool awaken;
        std::mutex handbrakeMutex;
        std::mutex sleepMutex;
        std::condition_variable waiting;

        void workerThread() {
            do {
                LOG(INFO) << "Worker " << index << " started execution";
                JobType* jobPtr;
                u32 id;

                while (provider(&jobPtr, &id)) {
                    jobPtr->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
                    notifier(jobPtr, id);
                }
                if (running) {
                    sleep();
                }
            } while (running);
            LOG(INFO) << "Worker " << index << " finished execution";
        }

    public:
        AbstractJobWorker(
            size_t index,
            bool autostart,
            un::JobProvider<JobType> provider,
            un::JobNotifier<JobType> notifier
        ) : index(index),
            thread(nullptr),
            waiting(),
            running(true),
            awaken(false),
            provider(provider),
            notifier(notifier) {
            if (autostart) {
                start();
            }
#ifdef WIN32
            int affinityMask = 1 << index;
            HANDLE hThread = reinterpret_cast<HANDLE>(thread->native_handle());
            auto returnStatus = SetThreadAffinityMask(hThread, affinityMask);
            if (returnStatus == 0) {
                LOG(INFO) << "Unable to set worker " << index << " affinity mask on Win32 (" << GetLastError() << ")";
            } else {
                LOG(INFO) << "Worker " << index << " has affinity mask " << affinityMask << " (Was " << returnStatus << ")";
            }
            std::string threadName = "UnnecessaryWorker-";
            threadName += std::to_string(index);
            SetThreadDescription(hThread, std::wstring(threadName.begin(), threadName.end()).c_str());
#endif
        }

        virtual ~AbstractJobWorker() {
            running = false;
        }

        bool isAwake() {
            return awaken;
        }

        void awake() {
            {
                std::lock_guard<std::mutex> lock(sleepMutex);
                if (!awaken) {
                    awaken = true;
                    waiting.notify_one();
                }
            }
        }

        void sleep() {
            {
                std::lock_guard<std::mutex> lock(sleepMutex);
                if (awaken) {
                    awaken = false;
                }
            }
            std::unique_lock<std::mutex> lock(handbrakeMutex);
            waiting.wait(lock);
        }

        void start() {
            thread = new std::thread(&AbstractJobWorker::workerThread, this);
        }
    };

    class JobWorker;

    class SimpleJob : public un::Job<JobWorker> {

    };

    class JobWorker : public un::AbstractJobWorker<un::SimpleJob> {
    public:
        JobWorker(
            size_t index,
            bool autostart,
            const JobProvider <JobType>& provider,
            const JobNotifier <JobType>& notifier
        );
    };
}
#endif