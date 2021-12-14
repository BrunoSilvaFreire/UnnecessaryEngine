#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/graphics/rendering_device.h>
#include <unnecessary/algorithm/dependency_graph.h>
#include <unnecessary/misc/types.h>
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
    private:
        std::string name = "Unnamed Job";
    public:
        virtual void operator()(un::JobWorker* worker) = 0;

        const std::string& getName() const;

        void setName(const std::string& newName);
    };

    class JobGraph : public un::DependencyGraph<un::Job*> {

    };



    class JobWorker {
    public:
        struct WorkerGraphicsResources {
        private:
            vk::CommandPool commandPool;
        public:
            WorkerGraphicsResources() = default;

            WorkerGraphicsResources(un::RenderingDevice& device);

            const vk::CommandPool& getCommandPool() const;

        };

    private:
        size_t index;
        std::thread* thread;
        JobSystem* jobSystem;
        bool running = true;
        bool awaken;
        std::mutex handbrakeMutex;
        std::mutex sleepMutex;
        std::condition_variable waiting;
        WorkerGraphicsResources graphicsResources;

        void workerThread();

    public:
        explicit JobWorker(
            un::Application& application,
            JobSystem* jobSystem,
            size_t index,
            bool start
        );

        JobWorker(JobWorker&& copy) noexcept;

        ~JobWorker();

        bool isAwake();

        void awake();

        void sleep();

        const WorkerGraphicsResources& getGraphicsResources() const;

        void start();
    };

    class JobSystem {
        friend class JobWorker;
    private:
        JobGraph tasks;


        std::vector<JobWorker> workers;
        std::queue<u32> awaitingExecution;
        std::mutex queueUsage, graphUsage;

        bool nextJob(Job** result, u32* id);

        void awakeSomeoneUp();

        void notifyCompletion(u32 id);


    public:
        u32 enqueue(Job* job, bool markForExecution);

        void addDependency(u32 from, u32 to);

        void markForExecution(u32 job);

        JobSystem(un::Application& application, int nThreads = -1);

        ~JobSystem();

        void start();

        template<typename T, typename ...Args>
        u32 enqueue(Args ...args) {
            return enqueue(new T(args...));
        }

        template<typename T, typename ...Args>
        u32 enqueue(u32 dependsOn, Args ...args) {
            return enqueue(dependsOn, new T(args...));
        }

        template<typename T>
        u32 then(u32 dependsOn, std::function<void(T&)> callback) {
            return enqueue(
                dependsOn, [&]() {
                    T* dependency = dynamic_cast<T*>(getJob(dependsOn));
                    if (dependency != nullptr) {
                        callback(dependency);
                    }
                }
            );
        }

        Job* getJob(u32 id);

        u32 enqueue(Job* job);

        u32 enqueue(u32 dependsOn, Job* job);

        int getNumWorkers();

        friend class World;
    };

    template<>
    std::string to_string<un::Job*>(un::Job* const& job);
}
#endif
