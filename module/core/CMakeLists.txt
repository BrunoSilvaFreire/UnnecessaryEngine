add_unnecessary_library(
    unnecessary_core

    MODULE
    core

    SOURCES
    include/unnecessary/def.h
    include/unnecessary/platform.h
    include/unnecessary/logging.h
    include/unnecessary/algorithms/n_tree.h
    include/unnecessary/algorithms/index_recycler.h
    include/unnecessary/memory/membuf.h
    include/unnecessary/memory/binary_writer.h
    include/unnecessary/memory/buffer_stream.h
    include/unnecessary/misc/event.h
    include/unnecessary/misc/types.h
    include/unnecessary/misc/historic.h
    include/unnecessary/misc/files.h
    include/unnecessary/misc/indexing.h
    include/unnecessary/misc/benchmark.h
    include/unnecessary/misc/pretty_print.h
    include/unnecessary/misc/list_printer.h
    include/unnecessary/misc/path_printer.h
    src/unnecessary/logging.cpp
    src/unnecessary/strings.cpp
    src/unnecessary/version.cpp
    src/unnecessary/misc/types.cpp
    src/unnecessary/misc/membuf.cpp
    src/unnecessary/misc/binary_writer.cpp
    src/unnecessary/misc/historic.cpp
    src/unnecessary/misc/benchmark.cpp
    src/unnecessary/misc/files.cpp
    src/unnecessary/misc/path_printer.cpp
    src/unnecessary/misc/pretty_print.cpp

    SOURCES_WIN
    src/unnecessary/strings.win.cpp

    TESTS
    tests/membuf_tests.cpp
    tests/historic_tests.cpp
    tests/template_tests.cpp
    tests/allocators_tests.cpp
    tests/tree_tests.cpp

    DEPENDENCIES
    termcolor::termcolor
    stduuid::stduuid
)
