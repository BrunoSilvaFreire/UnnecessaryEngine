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
    tools/unnecessary/serializer/writers/writer.h
    tools/unnecessary/serializer/writers/writer.cpp
    tools/unnecessary/serializer/writers/default_writer.h
    tools/unnecessary/serializer/writers/default_writer.cpp
    tools/unnecessary/serializer/writers/complex_writer.h
    tools/unnecessary/serializer/writers/complex_writer.cpp
    tools/unnecessary/serializer/writers/delegate_writer.h
    tools/unnecessary/serializer/writers/delegate_writer.cpp
    tools/unnecessary/serializer/writers/identifiable_writer.h
    tools/unnecessary/serializer/writers/identifiable_writer.cpp

    tools/unnecessary/serializer/writers/collection_writer.h
    tools/unnecessary/serializer/writers/collection_writer.cpp
    tools/unnecessary/serializer/generation.h
    tools/unnecessary/serializer/serializer_generator.cpp
    tools/unnecessary/serializer/generation.cpp
    tools/unnecessary/serializer/generation_plan.h
    tools/unnecessary/serializer/generation_plan.cpp
    tools/unnecessary/serializer/jobs/generate_serializer_job.h
    tools/unnecessary/serializer/jobs/generate_serializer_job.cpp
    tools/unnecessary/serializer/jobs/generate_includes_job.h
    tools/unnecessary/serializer/jobs/generate_includes_job.cpp

    DEPENDENCIES
    unnecessary_source_analysis
    unnecessary_graphs
    unnecessary_jobs
    grapphs_visualization
)
if (WIN32)
    message("Adding libclang_s")
    target_link_libraries(
        unnecessary_serializer_generator
        PUBLIC
        libclang_s
    )
endif ()
add_subdirectory(nlohmann)
