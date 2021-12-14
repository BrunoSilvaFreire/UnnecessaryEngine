#include <gtest/gtest.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/worker.h>
#include <unnecessary/jobs/job_chain.h>
#include "unnecessary/jobs/misc/load_file_job.h"


TEST(jobs, load_file) {
    un::SimpleJobSystem jobSystem(4);
    un::Buffer buf;
    {
        un::JobChain<un::SimpleJobSystem> jobChain(&jobSystem);
        jobChain.immediately<un::LoadFileJob>("resources/dummy.txt", &buf);
    }
}