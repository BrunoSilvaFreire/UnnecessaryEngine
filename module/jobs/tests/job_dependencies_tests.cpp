#include <gtest/gtest.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include "unnecessary/jobs/misc/load_file_job.h"
#include <cmath>

class DummyWorker : public un::AbstractJobWorker<un::SimpleJob> {
public:
    DummyWorker(
        size_t index,
        bool autostart,
        const un::JobProvider<JobType>& provider,
        const un::JobNotifier<JobType>& notifier
    ) : AbstractJobWorker(index, autostart, provider, notifier) {}
};

typedef un::JobSystem<un::JobWorker, DummyWorker> DummyJobSystem;

TEST(jobs, openness) {
    const std::size_t numTries = 1024;
    const std::size_t numWorkers = std::thread::hardware_concurrency();
    const std::size_t numJobs = numWorkers * 4;
    for (std::size_t i = 0; i < numTries; ++i) {

        un::SimpleJobSystem jobSystem(numWorkers, true);
        std::atomic<std::size_t> numCompletedJobs = 0;
        {
            un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
            for (std::size_t j = 0; j < numJobs; ++j) {
                chain.immediately<un::LambdaJob<un::JobWorker>>(
                    [=, &numCompletedJobs]() {
                        std::chrono::milliseconds workTime(5);
                        std::this_thread::sleep_for(workTime);
                        numCompletedJobs++;
                    }
                );
            }
        }
        jobSystem.complete();
        LOG(INFO) << "Execution " << i << ": " << numJobs << " vs " << numCompletedJobs;
        ASSERT_EQ(numJobs, numCompletedJobs);
    }
}


TEST(jobs, load_file) {
    un::SimpleJobSystem jobSystem(4, true);
    un::Buffer buf;
    {
        un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
        un::JobHandle loadHandle, lambdaHandle;
        std::filesystem::path filePath = "resources/dummy.txt";
        chain.immediately<un::LoadFileJob>(
            &loadHandle,
            std::filesystem::absolute(filePath),
            &buf,
            std::ios::in
        );
        chain.finally(
            [&]() {
                char* string = reinterpret_cast<char*>(buf.data());
//                LOG(INFO) << "Finished reading file: " << string;
                ASSERT_STREQ(string, "Hello World!");
            }
        );
    }
    jobSystem.complete();
}

void log_index(size_t index) {
    LOG(INFO) << "Ran " << index;
}

TEST(jobs, sequence_test) {
    un::SimpleJobSystem jobSystem(2, true);
    un::Buffer buf;

    {
        un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
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

TEST(jobs, benchmark) {
    un::SimpleJobSystem jobSystem(std::thread::hardware_concurrency(), true);
    const size_t numJobs = std::pow(2, 16);
    un::Buffer buf(numJobs, false);

    {
        un::JobChain<un::SimpleJobSystem> chain(&jobSystem);

        for (size_t i = 0; i < numJobs; i++) {
            chain.immediately<un::LambdaJob<>>(
                [&, i]() {
                    int r;
                    for (int j = 0; j < 2000; ++j) {
                        r = rand();
                    }
                    buf[i] = r;
                }
            );
        }

    }
    jobSystem.complete();
}