setup_libast()

add_unnecessary_library(
    unnecessary_source_analysis

    SOURCES
    include/unnecessary/source_analysis/structures.h
    include/unnecessary/source_analysis/unnecessary_logger.h
    include/unnecessary/source_analysis/parse_plan.h
    include/unnecessary/source_analysis/jobs/parse_file_job.h
    include/unnecessary/source_analysis/cppast/transpiler.h
    include/unnecessary/source_analysis/cppast/transpilation.h
    src/unnecessary/source_analysis/structures.cpp
    src/unnecessary/source_analysis/unnecessary_logger.cpp
    src/unnecessary/source_analysis/parse_plan.cpp
    src/unnecessary/source_analysis/jobs/parse_file_job.cpp
    src/unnecessary/source_analysis/cppast/transpiler.cpp
    src/unnecessary/source_analysis/cppast/transpilation.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_jobs
    cppast

    TESTS
    tests/multithread_parsing.cpp
)