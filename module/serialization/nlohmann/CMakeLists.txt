add_unnecessary_library(
    unnecessary_nlohmann_archiver

    MODULE serialization

    SOURCES
    include/unnecessary/nlohmann/adapter.h
    include/unnecessary/nlohmann/bson_archiver.h
    include/unnecessary/nlohmann/json_archiver.h
    src/unnecessary/nlohmann/adapter.cpp
    src/unnecessary/nlohmann/bson_archiver.cpp
    src/unnecessary/nlohmann/json_archiver.cpp

    DEPENDENCIES
    unnecessary_serialization
    nlohmann_json::nlohmann_json

    TESTS
    tests/bson_tests.cpp
    tests/json_tests.cpp
)