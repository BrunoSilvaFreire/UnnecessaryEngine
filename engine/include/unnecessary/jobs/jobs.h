#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/algorithm/graphs/adjacency_list.h>
#include <utility>
#include <chrono>
#include <thread>
#include <semaphore>
#include <mutex>
#include <condition_variable>

#define READ_ONLY()
#define READ_WRITE()
#define WRITE_ONLY()

namespace un {
    typedef u64 JobHandle;

    class Job {
    public:
        virtual explicit operator void() = 0;
    };

    enum JobDependencyType : u8 {
        eNone = 0,
        /**
         * Marks that THIS JOB is a requirement of the OTHER JOB
         * (Used to find all jobs that may be started by the completion on this job)
         */
        eRequirement = 1,
        /**
         * Marks that the OTHER JOB is required by THIS JOB
         * (Used to check whether all dependency of this job have been solved)
         */
        eRequired = 2
    };

    typedef un::AdjacencyList<Job *, JobDependencyType> JobGraph;

    class LambdaJob : public Job {
    public:
        typedef std::function<void()> Callback;
    private:
        Callback callback;
    public:
        LambdaJob(Callback callback);

        explicit operator void() override;
    };

    class JobSystem {
    private:
        JobGraph tasks;

        class JobWorker;

        std::vector<JobWorker> threads;
        std::queue<u32> awaitingExecution;
        std::mutex queueUsage, graphUsage;

        bool nextJob(Job **result, u32 *id);

        void awakeSomeoneUp();

        void notifyCompletion(u32 id);


        class JobWorker {
        private:
            std::thread *thread;
            JobSystem *jobSystem;
            bool running = true;
            bool awaken;
            std::mutex mutex;
            std::condition_variable waiting;

            void workerThread();

        public:
            explicit JobWorker(JobSystem *jobSystem);

            JobWorker(JobWorker &&copy) noexcept;

            ~JobWorker();

            bool isAwake();

            void awake();

            void sleep();
        };


    public:
        u32 enqueue(Job *job, bool markForExecution);

        void addDependency(u32 from, u32 to);

        void markForExecution(u32 job);

        JobSystem();

        ~JobSystem();

        template<typename T, typename ...Args>
        u32 enqueue(Args ...args) {
            return enqueue(new T(args...));
        }

        template<typename T, typename ...Args>
        u32 enqueue(u32 dependsOn, Args ...args) {
            return enqueue(dependsOn, new T(args...));
        }

        template<typename T>
        u32 then(u32 dependsOn, std::function<void(T &)> callback) {
            return enqueue(dependsOn, [&]() {
                T *dependency = dynamic_cast<T *>(getJob(dependsOn));
                if (dependency != nullptr) {
                    callback(dependency);
                }
            });
        }

        Job *getJob(u32 id);

        u32 enqueue(Job *job);

        u32 enqueue(u32 dependsOn, Job *job);

        u32 enqueue(un::LambdaJob::Callback callback);

        u32 enqueue(u32 dependsOn, un::LambdaJob::Callback callback);
    };
}
#endif
