#include <gtest/gtest.h>
#include <grapphs/dot.h>
#include <grapphs/tests/mazes.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/jobs/misc/file_jobs.h>
#include <unnecessary/jobs/misc/graph_exploration.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/misc/benchmark.h>
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
        un::job_system_builder<un::simple_job_system> builder;
        builder.with_logger();
        builder.with_recorder();
        auto jobSystem = builder.build();
        std::atomic<std::size_t> numCompletedJobs = 0;
        {
            un::job_chain<un::simple_job_system> chain(jobSystem.get());
            for (std::size_t j = 0; j < numJobs; ++j) {
                un::job_handle handle;
                std::size_t sleepTime = msRange(engine);
                chain.immediately<un::lambda_job<un::job_worker>>(
                    &handle,
                    [=, &numCompletedJobs]() {
                        std::chrono::milliseconds workTime(sleepTime);
                        std::this_thread::sleep_for(workTime);
                        ++numCompletedJobs;
                    }
                );
                std::stringstream name;
                name << "Simulated Job (" << std::to_string(sleepTime) << "ms)";
                chain.set_name(handle, name.str());
            }
        }
        jobSystem->start();
        jobSystem->complete();
        ASSERT_EQ(numJobs, numCompletedJobs);
    }
}

TEST(jobs, load_file) {
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_logger();
    builder.with_recorder();
    auto jobSystem = builder.build();
    un::byte_buffer buf;
    {
        un::job_chain<un::simple_job_system> chain(jobSystem.get());
        un::job_handle loadHandle;
        std::filesystem::path filePath = "resources/dummy.txt";
        chain.immediately<un::load_file_job>(
            &loadHandle,
            absolute(filePath),
            &buf,
            std::ios::in
        );
        chain.finally(
            [&]() {
                auto string = reinterpret_cast<char*>(buf.data());
                //                LOG(INFO) << "Finished reading file: " << string;
                ASSERT_STREQ(string, "Hello World!");
            }
        );
    }
    jobSystem->complete();
}

struct exploration_vertex {
public:
    std::size_t randomData;

    friend std::ostream& operator<<(std::ostream& os, const exploration_vertex& vertex) {
        os << "randomData: " << vertex.randomData;
        return os;
    }
};

using dummy_exploration_graph = gpp::adjacency_list<exploration_vertex, float>;

#define NUM_GRAPH_ENTRIES 2000

void populate(dummy_exploration_graph& graph) {
    std::random_device device;

    graph.reserve(NUM_GRAPH_ENTRIES);
    for (int i = 0; i < NUM_GRAPH_ENTRIES; ++i) {
        exploration_vertex vertex;
        vertex.randomData = device();
        graph.push(vertex);
    }
    for (size_t x = 0; x < NUM_GRAPH_ENTRIES; ++x) {
        for (size_t y = 0; y < NUM_GRAPH_ENTRIES; ++y) {
            if (device()) {
                uint32_t data = device();
                float edge = *reinterpret_cast<float*>(&data);
                graph.connect(x, y, edge);
            }
        }
    }
}

TEST(jobs, graph_exploration) {
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_logger();
    builder.with_recorder();
    auto jobSystem = builder.build();
    gpp::test_mazes(
        [&](gpp::Maze& maze) {
            const gpp::adjacency_list<gpp::Cell, int>& graph = maze.get_graph();
            gpp::save_to_dot(
                graph,
                std::filesystem::current_path() / "exploration_graph.dot"
            );
            LOG(INFO) << "Exploring maze " << maze.get_size();
            std::set<std::size_t> alreadyExplored;
            un::graph_explorer<gpp::adjacency_list<gpp::Cell, int>> explorer(
                graph,
                [&](u32 index, const gpp::Cell& vertex) {
                    bool isDuplicate = alreadyExplored.contains(index);
                    if (!isDuplicate) {
                        LOG(INFO) << index << " OK";
                    }
                    else {
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
                /*un::JobChain<un::simple_job_system> chain(jobSystem.get());
                chain.immediately<un::ExploreGraphVertexJob<gpp::adjacency_list<gpp::Cell, int>, un::simple_job_system>>(
                    maze.getStart(),
                    &graph,
                    &explorer,
                    &jobSystem,
                    true
                );*/
            }
            explorer.complete();
            LOG(INFO) << "Exploration _completed!";
        }
    );

    jobSystem->complete();
}

TEST(jobs, set_affinity) {
    std::size_t numWorkers = std::thread::hardware_concurrency();
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_logger();
    builder.with_recorder();
    auto jobSystem = builder.build();
    auto& workers = jobSystem->get_worker_pool<un::job_worker>().get_workers();
    for (std::size_t i = 0; i < numWorkers; ++i) {
        un::thread* thread = workers[i]->get_thread();
        EXPECT_TRUE(thread->setCore(i)) << "Unable to set thread " << i << " affinity";
    }
    jobSystem->complete();
}

void log_index(size_t index) {
    LOG(INFO) << "Ran " << index;
}

TEST(jobs, sequence_test) {
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_logger();
    builder.with_recorder();
    auto jobSystem = builder.build();
    un::byte_buffer buf;

    {
        un::job_chain<un::simple_job_system> chain(jobSystem.get());
        un::job_handle first, second;
        chain.immediately<un::lambda_job<>>(
            &first,
            [&]() {
                log_index(0);
            }
        );
        chain.immediately<un::lambda_job<>>(
            &second,
            []() {
                log_index(1);
            }
        );
        for (size_t i = 0; i < 20000; i += 2) {
            chain.after<un::lambda_job<>>(
                first,
                &first,
                [=]() {
                    log_index(i);
                }
            );
            chain.after<un::lambda_job<>>(
                second,
                &second,
                [=]() {
                    log_index(i + 1);
                }
            );
        }
    }
    jobSystem->complete();
}

class populate_buffer_job final : public un::parallel_for_job<un::job_worker> {
private:
    std::shared_ptr<un::byte_buffer> _buf;

public:
    explicit populate_buffer_job(
        std::shared_ptr<un::byte_buffer> buf
    ) : _buf(std::move(buf)) {
    }

    UN_AGGRESSIVE_INLINE void operator()(size_t index, un::job_worker* worker) override {
        int r;
        for (int j = 0; j < 2000; ++j) {
            r = rand();
        }
        _buf->operator[](index) = r;
    }
};

TEST(jobs, benchmark) {
    const auto bufferSize = static_cast<size_t>(std::pow(2, 20));
    GTEST_LOG_(INFO) << "Generating buffer of length " << bufferSize;
    for (int i = std::thread::hardware_concurrency(); i > 0; --i) {
        un::chronometer<std::chrono::nanoseconds> chronometer;
        un::job_system_builder<un::simple_job_system> builder;
        builder.with_logger();
        builder.with_recorder();
        auto jobSystem = builder.build();

        auto buf = std::make_shared<un::byte_buffer>(bufferSize, false);
        {
            un::job_chain<un::simple_job_system> chain(jobSystem.get());

            populate_buffer_job::parallelize(
                new populate_buffer_job(buf),
                chain,
                bufferSize,
                bufferSize / i
            );
        }
        jobSystem->complete();
        std::chrono::nanoseconds duration = chronometer.stop();
        GTEST_LOG_(INFO) << "Parallel buffer population took " << duration.count() << "ns ("
                         << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
                         << "ms) with "
                         << i << " threads.";
    }
}
