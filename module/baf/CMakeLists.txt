# Note: baf stands for Big Ass File
add_unnecessary_library(
    unnecessary_baf

    SOURCES
    include/unnecessary/baf/baf_file.h
    src/unnecessary/baf/baf_file.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_jobs
)
add_unnecessary_tool(
    unnecessary_baffer

    SOURCES
    tools/unnecessary/baf/baffer.cpp

    DEPENDENCIES
    unnecessary_baf
)