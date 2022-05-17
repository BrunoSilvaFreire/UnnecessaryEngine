setup_libast()

add_unnecessary_library(
    unnecessary_source_analysis

    SOURCES
    include/unnecessary/source_analysis/structures.h
    include/unnecessary/source_analysis/parsing.h
    include/unnecessary/source_analysis/parser.h
    src/unnecessary/source_analysis/structures.cpp
    src/unnecessary/source_analysis/parsing.cpp
    src/unnecessary/source_analysis/parser.cpp

    DEPENDENCIES
    unnecessary_core
    cppast
)