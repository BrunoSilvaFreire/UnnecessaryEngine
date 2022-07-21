setup_libast()

add_unnecessary_library(
    unnecessary_source_analysis

    SOURCES
    include/unnecessary/source_analysis/structures.h
    include/unnecessary/source_analysis/parsing.h
    include/unnecessary/source_analysis/parser.h
    include/unnecessary/source_analysis/unnecessary_logger.h
    include/unnecessary/source_analysis/parse_plan.h
    include/unnecessary/source_analysis/jobs/parse_file_job.h
    src/unnecessary/source_analysis/structures.cpp
    src/unnecessary/source_analysis/parsing.cpp
    src/unnecessary/source_analysis/parser.cpp
    src/unnecessary/source_analysis/unnecessary_logger.cpp
    src/unnecessary/source_analysis/parse_plan.cpp
    src/unnecessary/source_analysis/jobs/parse_file_job.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_jobs
    cppast

    TESTS
    tests/multithread_parsing.cpp
)