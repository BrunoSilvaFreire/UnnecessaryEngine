add_unnecessary_module(
    unnecessary_atlas

    SOURCES
    include/unnecessary/atlas/atlas.h
    include/unnecessary/atlas/atlas_serialization.h
    src/unnecessary/atlas/atlas_serialization.cpp

    DEPENDENCIES
    glm::glm
    unnecessary_jobs
    unnecessary_serialization
)