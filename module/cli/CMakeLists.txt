add_unnecessary_library(
    unnecessary_cli

    SOURCES
    include/unnecessary/cli/pretty_print.h
    include/unnecessary/cli/list_printer.h
    include/unnecessary/cli/path_printer.h
    src/unnecessary/cli/cli.cpp
    src/unnecessary/cli/path_printer.cpp

    DEPENDENCIES
    cxxopts::cxxopts
    unnecessary_core
)