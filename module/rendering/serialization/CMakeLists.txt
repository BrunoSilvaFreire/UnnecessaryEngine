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
    GLOBAL_INCLUDES
    unnecessary/rendering/serialization/pipeline_stage_description_serializer.h

    RELATIVE_TO
    ../include/

    FILES
    ../include/unnecessary/rendering/pipelines/pipeline_description.h
)