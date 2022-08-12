add_unnecessary_library(
    unnecessary_cli

    SOURCES
    src/unnecessary/cli/cli.cpp

    DEPENDENCIES
    cxxopts::cxxopts
    unnecessary_core
)