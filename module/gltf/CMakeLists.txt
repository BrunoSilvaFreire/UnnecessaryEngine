
add_unnecessary_library(
    unnecessary_gltf
    MODULE graphics

    SOURCES
    include/unnecessary/gltf/load_gltf_job.h
    src/unnecessary/gltf/load_gltf_job.cpp

    DEPENDENCIES
    unnecessary_jobs
    unnecessary_rendering
    glm::glm
    TinyGLTF::TinyGLTF

    TESTS
    tests/load_dragon.cpp
)