#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <grapphs/adjacency_list.h>
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
    class Application;

    typedef u64 JobHandle;

    class JobSystem;

    class JobWorker;


    class Job {
    public:
        virtual void operator()(un::JobWorker *worker) = 0;
    };

    enum JobDependencyType : u8 {
        eNone = 0,
        /**
         * Marks that the OTHER JOB is a requirement of THIS JOB
         * (Used to check whether all dependency of this job have been solved)
         */
        eRequirement = 1,
        /**
         * Marks that the THIS JOB is required by the OTHER JOB
         * (Used to find all jobs that may be started by the completion on this job)
         */
        eRequired = 2
    };

    typedef gpp::AdjacencyList<Job *, JobDependencyType, u32> JobGraph;

    class LambdaJob : public Job {
    public:
        typedef std::function<void(un::JobWorker *worker)> Callback;
        typedef std::function<void()> VoidCallback;
    private:
        Callback callback;
        VoidCallback voidCallback;
    public:
        LambdaJob(const Callback &callback);

        LambdaJob(const VoidCallback &callback);

        void operator()(un::JobWorker *worker) override;
    };


    class JobWorker {
    private:
        size_t index;
        std::thread *thread;
        JobSystem *jobSystem;
        bool running = true;
        bool awaken;
        std::mutex handbrakeMutex;
        std::mutex sleepMutex;
        std::condition_variable waiting;
        vk::CommandPool commandPool;

        void workerThread();

    public:
        explicit JobWorker(un::Application &application, JobSystem *jobSystem, size_t index);

        JobWorker(JobWorker &&copy) noexcept;

        ~JobWorker();

        bool isAwake();

        void awake();

        void sleep();

        vk::CommandPool getCommandBufferPool() const;
    };

    class JobSystem {
        friend class JobWorker;

    private:
        JobGraph tasks;


        std::vector<JobWorker> workers;
        std::queue<u32> awaitingExecution;
        std::mutex queueUsage, graphUsage;

        bool nextJob(Job **result, u32 *id);

        void awakeSomeoneUp();

        void notifyCompletion(u32 id);


    public:
        u32 enqueue(Job *job, bool markForExecution);

        void addDependency(u32 from, u32 to);

        void markForExecution(u32 job);

        JobSystem(un::Application &application);

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

        u32 enqueue(u32 dependsOn, const un::LambdaJob::Callback& callback);

        int getNumWorkers();
    };
}
#endif
