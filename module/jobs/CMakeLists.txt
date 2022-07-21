
add_unnecessary_library(
    unnecessary_jobs

    MODULE core

    SOURCES
    include/unnecessary/jobs/job.h
    include/unnecessary/jobs/job_system.h
    include/unnecessary/jobs/job_chain.h
    include/unnecessary/jobs/job_system_builder.h
    include/unnecessary/jobs/simple_jobs.h
    include/unnecessary/jobs/thread.h
    include/unnecessary/jobs/parallel_for_job.h
    include/unnecessary/jobs/misc/file_jobs.h
    include/unnecessary/jobs/misc/join_buffers_job.h
    include/unnecessary/jobs/misc/lambda_job.h
    include/unnecessary/jobs/misc/graph_exploration.h
    include/unnecessary/jobs/misc/fence.h
    include/unnecessary/jobs/workers/worker.h
    include/unnecessary/jobs/recorder/job_system_recorder.h
    include/unnecessary/jobs/recorder/data/recorder_events.h
    include/unnecessary/jobs/recorder/worker_recorder.h
    include/unnecessary/jobs/recorder/data/event_history.h
    include/unnecessary/jobs/recorder/data/events.h
    include/unnecessary/jobs/logger/job_system_logger.h
    src/unnecessary/jobs/jobs.cpp
    src/unnecessary/jobs/job_system_builder.cpp
    src/unnecessary/jobs/misc/file_jobs.cpp
    src/unnecessary/jobs/misc/join_buffers_job.cpp
    src/unnecessary/jobs/parallel_for_job.cpp
    src/unnecessary/jobs/worker.cpp
    src/unnecessary/jobs/job_dispatch.cpp
    src/unnecessary/jobs/simple_jobs.cpp
    src/unnecessary/jobs/thread.cpp
    src/unnecessary/jobs/recorder/data/event_history.cpp
    src/unnecessary/jobs/recorder/job_system_recorder.cpp
    src/unnecessary/jobs/recorder/data/recorder_events.cpp
    src/unnecessary/jobs/recorder/data/events.cpp

    SOURCES_APPLE
    src/unnecessary/jobs/platform/thread.apple.cpp

    SOURCES_LINUX
    src/unnecessary/jobs/platform/thread.linux.cpp

    SOURCES_WIN
    src/unnecessary/jobs/platform/thread.win.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_graphs

    TESTS
    tests/job_dependencies_tests.cpp
    tests/dynamic_job_tests.cpp
)
target_link_libraries(
    unnecessary_jobs_tests
    PUBLIC
    grapphs_visualization
    grapphs_testlib
)
unnecessary_target_resources(
    TARGET unnecessary_jobs_tests
    RESOURCES
    resources/dummy.txt
)

set(JOBS_MAZES_SIZES 30 50 100)
add_custom_target(
    unnecessary_jobs_generate_mazes
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_BINDIR}/mazes
)

foreach (INDEX IN LISTS JOBS_MAZES_SIZES)
    set(MAZE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mazes/maze_${INDEX}.json)
    message(VERBOSE "Discovered maze: ${MAZE_OUTPUT}")
    add_custom_command(
        TARGET
        unnecessary_jobs_generate_mazes
        COMMAND
        ${Python_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/thirdparty/grapphs/scripts/maze_gen.py
        --size ${INDEX}
        --monteMazes 10
        --monteAttempts 10
        --output ${MAZE_OUTPUT}
        --reuse true
        BYPRODUCTS ${MAZE_OUTPUT}
        COMMENT "Generating maze of size ${INDEX}..."

    )
endforeach ()
add_dependencies(
    unnecessary_jobs_tests
    unnecessary_jobs_generate_mazes
)