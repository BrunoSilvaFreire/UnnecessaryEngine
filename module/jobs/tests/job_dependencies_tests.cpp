#include <gtest/gtest.h>
#include <grapphs/dot.h>
#include <grapphs/tests/mazes.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/file_jobs.h>
#include <unnecessary/jobs/misc/graph_exploration.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>
#include <unnecessary/misc/benchmark.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <cmath>
#include <random>
#include <ostream>
#include <utility>

TEST(jobs, completeness) {
    const std::size_t numTries = 1 << 8;
    const std::size_t numWorkers = std::thread::hardware_concurrency();
    const std::size_t numJobs = numWorkers * 8;
    const std::size_t minDelay = 1;
    const std::size_t maxDelay = 20;
    std::uniform_int_distribution<std::size_t> msRange(minDelay, maxDelay);
    std::default_random_engine engine;
    for (std::size_t i = 0; i < numTries; ++i) {

        SimpleJobSystem jobSystem(numWorkers, false);
        un::JobSystemRecorder<SimpleJobSystem> recorder(&jobSystem);
        std::atomic<std::size_t> numCompletedJobs = 0;
        {
            un::JobChain<SimpleJobSystem> chain(&jobSystem);
            for (std::size_t j = 0; j < numJobs; ++j) {
                un::JobHandle handle;
                std::size_t sleepTime = msRange(engine);
                chain.immediately<un::LambdaJob<JobWorker>>(
                    &handle,
                    [=, &numCompletedJobs]() {
                        std::chrono::milliseconds workTime(sleepTime);
                        std::this_thread::sleep_for(workTime);
                        numCompletedJobs++;
                    }
                );
                std::stringstream name;
                name << "Simulated Job (" << std::to_string(sleepTime) << "ms)";
                chain.setName(handle, name.str());
            }
        }
        jobSystem.start();
        jobSystem.complete();
        ASSERT_EQ(numJobs, numCompletedJobs);
        auto outputFile = std::filesystem::current_path() /
                          (std::string("execution_") + std::to_string(i) + ".csv");
        std::ofstream stream(outputFile, std::ios::out);
        stream << recorder.toCSV();
        stream.close();
    }
}


TEST(jobs, load_file) {
    SimpleJobSystem jobSystem(4, true);
    un::Buffer buf;
    {
        un::JobChain<SimpleJobSystem> chain(&jobSystem);
        un::JobHandle loadHandle;
        std::filesystem::path filePath = "resources/dummy.txt";
        chain.immediately<un::LoadFileJob>(
            &loadHandle,
            std::filesystem::absolute(filePath),
            &buf,
            std::ios::in
        );
        chain.finally(
            [&]() {
                char *string = reinterpret_cast<char *>(buf.data());
//                LOG(INFO) << "Finished reading file: " << string;
                ASSERT_STREQ(string, "Hello World!");
            }
        );
    }
    jobSystem.complete();
}

struct ExplorationVertex {
public:
    std::size_t randomData;

    friend std::ostream &operator<<(std::ostream &os, const ExplorationVertex &vertex) {
        os << "randomData: " << vertex.randomData;
        return os;
    }
};

typedef gpp::AdjacencyList<ExplorationVertex, float> DummyExplorationGraph;

#define NUM_GRAPH_ENTRIES 2000

void populate(DummyExplorationGraph &graph) {
    std::random_device device;

    graph.reserve(NUM_GRAPH_ENTRIES);
    for (int i = 0; i < NUM_GRAPH_ENTRIES; ++i) {
        ExplorationVertex vertex;
        vertex.randomData = device();
        graph.push(vertex);
    }
    for (size_t x = 0; x < NUM_GRAPH_ENTRIES; ++x) {
        for (size_t y = 0; y < NUM_GRAPH_ENTRIES; ++y) {
            if (device()) {
                uint32_t data = device();
                float edge = *reinterpret_cast<float *>(&data);
                graph.connect(x, y, edge);
            }
        }
    }
}

TEST(jobs, graph_exploration) {


    SimpleJobSystem jobSystem(4, true);
    gpp::test_mazes(
        [&](gpp::Maze &maze) {
            const gpp::AdjacencyList<gpp::Cell, int> &graph = maze.getGraph();
            gpp::save_to_dot(
                graph,
                std::filesystem::current_path() / "exploration_graph.dot"
            );
            LOG(INFO) << "Exploring maze " << maze.getSize();
            std::set<std::size_t> alreadyExplored;
            un::GraphExplorer<gpp::AdjacencyList<gpp::Cell, int>> explorer(
                graph,
                [&](u32 index, const gpp::Cell &vertex) {
                    bool isDuplicate = alreadyExplored.contains(index);
                    if (!isDuplicate) {
                        LOG(INFO) << index << " OK";
                    } else {
                        LOG(FUCK) << index << " Duplicate";
                    }
                    ASSERT_FALSE(isDuplicate);
                    alreadyExplored.emplace(index);
                },
                [](u32 from, u32 to, int edge) {
                    LOG(INFO) << from << " -> " << to;
                }
            );

            {
                un::JobChain<SimpleJobSystem> chain(&jobSystem);
                chain.immediately<un::ExploreGraphVertexJob<gpp::AdjacencyList<gpp::Cell, int>, SimpleJobSystem>>(
                    maze.getStart(),
                    &graph,
                    &explorer,
                    &jobSystem,
                    true
                );
            }
            explorer.complete();
            LOG(INFO) << "Exploration completed!";
        }
    );

    jobSystem.complete();
}

TEST(jobs, set_affinity) {
    std::size_t numWorkers = std::thread::hardware_concurrency();
    SimpleJobSystem jobSystem(numWorkers, true);
    auto &workers = jobSystem.getWorkerPool<JobWorker>().getWorkers();
    for (std::size_t i = 0; i < numWorkers; ++i) {
        un::Thread *thread = workers[i]->getThread();
        EXPECT_TRUE(thread->setCore(i)) << "Unable to set thread " << i << " affinity";
    }
    jobSystem.complete();
}

void log_index(size_t index) {
    LOG(INFO) << "Ran " << index;
}

TEST(jobs, sequence_test) {
    SimpleJobSystem jobSystem(2, true);
    un::Buffer buf;

    {
        un::JobChain<SimpleJobSystem> chain(&jobSystem);
        un::JobHandle first, second;
        chain.immediately<un::LambdaJob<>>(
            &first,
            [&]() {
                log_index(0);
            }
        );
        chain.immediately<un::LambdaJob<>>(
            &second,
            []() {
                log_index(1);
            }
        );
        for (size_t i = 0; i < 20000; i += 2) {
            chain.after<un::LambdaJob<>>(
                first,
                &first,
                [=]() {
                    log_index(i);
                }
            );
            chain.after<un::LambdaJob<>>(
                second,
                &second,
                [=]() {
                    log_index(i + 1);
                }
            );
        }

    }
    jobSystem.complete();
}

class PopulateBufferJob final : public un::ParallelForJob<JobWorker> {
private:
    std::shared_ptr<un::Buffer> buf;
public:
    explicit PopulateBufferJob(
        std::shared_ptr<un::Buffer> buf
    ) : buf(std::move(buf)) {}

    UN_AGGRESSIVE_INLINE void operator()(size_t index, JobWorker *worker) override {
        int r;
        for (int j = 0; j < 2000; ++j) {
            r = rand();
        }
        buf->operator[](index) = r;
    }
};

TEST(jobs, benchmark) {
    const auto bufferSize = static_cast<size_t>(std::pow(2, 20));
    GTEST_LOG_(INFO) << "Generating buffer of length " << bufferSize;
    for (int i = std::thread::hardware_concurrency(); i > 0; --i) {
        un::Chronometer<std::chrono::nanoseconds> chronometer;
        SimpleJobSystem jobSystem(i, true);

        auto buf = std::make_shared<un::Buffer>(bufferSize, false);
        {
            un::JobChain<SimpleJobSystem> chain(&jobSystem);

            PopulateBufferJob::parallelize(
                new PopulateBufferJob(buf),
                chain,
                bufferSize,
                bufferSize / i
            );
        }
        jobSystem.complete();
        std::chrono::nanoseconds duration = chronometer.stop();
        GTEST_LOG_(INFO) << "Parallel buffer population took " << duration.count() << "ns ("
                         << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms) with "
                         << i << " threads.";
    }

}