add_unnecessary_module(
        unnecessary_core

        SOURCES
        include/unnecessary/def.h
        include/unnecessary/logging.h
        include/unnecessary/misc/event.h
        include/unnecessary/misc/types.h
        include/unnecessary/misc/membuf.h
        include/unnecessary/misc/binary_writer.h
        include/unnecessary/misc/historic.h
        include/unnecessary/misc/benchmark.h
        src/unnecessary/logging.cpp
        src/unnecessary/strings.cpp
        src/unnecessary/version.cpp
        src/unnecessary/misc/types.cpp
        src/unnecessary/misc/membuf.cpp
        src/unnecessary/misc/binary_writer.cpp
        src/unnecessary/misc/historic.cpp
        src/unnecessary/misc/benchmark.cpp

        TESTS
        tests/membuf_tests.cpp
        tests/historic_tests.cpp
        tests/template_tests.cpp

        DEPENDENCIES
        termcolor::termcolor
)
