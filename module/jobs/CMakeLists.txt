
add_unnecessary_module(
    unnecessary_jobs

    SOURCES
    include/unnecessary/jobs/job.h
    include/unnecessary/jobs/job_system.h
    include/unnecessary/jobs/job_chain.h
    include/unnecessary/jobs/misc/load_file_job.h
    include/unnecessary/jobs/parallel_for_job.h
    include/unnecessary/jobs/misc/lambda_job.h
    include/unnecessary/jobs/worker.h
    src/unnecessary/jobs/jobs.cpp
    src/unnecessary/jobs/load_file_job.cpp
    src/unnecessary/jobs/parallel_for_job.cpp
    src/unnecessary/jobs/worker.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_graphs

    TESTS
    tests/job_dependencies_tests.cpp
)