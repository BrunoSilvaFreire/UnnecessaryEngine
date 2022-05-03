add_unnecessary_library(
    unnecessary_serialization

    MODULE serialization

    SOURCES
    include/unnecessary/serialization/serialized.h
    include/unnecessary/serialization/serialization_types.h
    include/unnecessary/serialization/serialized_array.h
    include/unnecessary/serialization/serialized_primitive.h
    include/unnecessary/serialization/serialized_string.h
    include/unnecessary/archives/archiver.h

    src/unnecessary/serialization/types/serialized.cpp
    src/unnecessary/serialization/types/serialized_array.cpp
    src/unnecessary/serialization/types/serialized_primitive.cpp
    src/unnecessary/serialization/types/serialized_string.cpp

    DEPENDENCIES
    unnecessary_core
    stduuid::stduuid

    TESTS
    tests/serialization_tests.cpp
)
add_unnecessary_tool(
    unnecessary_serializer_generator

    SOURCES
    tools/unnecessary/serializer/serializer_generator.cpp
)
add_subdirectory(nlohmann)