#include <gtest/gtest.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include "unnecessary/jobs/misc/load_file_job.h"

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
    un::SimpleJobSystem jobSystem(4, true);
    const std::size_t numJobs = 4;
    std::size_t numCompletedJobs = 0;
    {
        un::JobChain<un::SimpleJobSystem> chain(&jobSystem);
        for (std::size_t i = 0; i < numJobs; ++i) {
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
    ASSERT_EQ(numJobs, numCompletedJobs);
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

void log(size_t index) {
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
                log(0);
            }
        );
        chain.immediately<un::LambdaJob<>>(
            &second,
            []() {
                log(1);
            }
        );
        for (size_t i = 0; i < 20000; i += 2) {
            chain.after<un::LambdaJob<>>(
                first,
                &first,
                [=]() {
                    log(i);
                }
            );
            chain.after<un::LambdaJob<>>(
                second,
                &second,
                [=]() {
                    log(i + 1);
                }
            );
        }

    }
    jobSystem.stop(true);
}