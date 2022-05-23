setup_libast()

add_unnecessary_library(
    unnecessary_source_analysis

    SOURCES
    include/unnecessary/source_analysis/structures.h
    include/unnecessary/source_analysis/parsing.h
    include/unnecessary/source_analysis/parser.h
    include/unnecessary/source_analysis/unnecessary_logger.h
    src/unnecessary/source_analysis/structures.cpp
    src/unnecessary/source_analysis/parsing.cpp
    src/unnecessary/source_analysis/parser.cpp
    src/unnecessary/source_analysis/unnecessary_logger.cpp
    DEPENDENCIES
    unnecessary_core
    cppast
)