add_unnecessary_library(
    unnecessary_rendering_serialization

    SOURCES
    include/unnecessary/rendering/serialization/rendering_serialization.h
    src/unnecessary/rendering/serialization/rendering_serialization.cpp
    DEPENDENCIES
    unnecessary_rendering
    unnecessary_serialization
)
add_unnecessary_serialization(
    TARGET unnecessary_rendering_serialization

    RELATIVE_TO
    ${CMAKE_SOURCE_DIR}/module/rendering/include/

    FILES
    ../include/unnecessary/rendering/pipelines/pipeline_description.h
)
