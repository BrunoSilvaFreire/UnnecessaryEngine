add_unnecessary_library(
    unnecessary_math
    MODULE core

    SOURCES
    include/unnecessary/math/rect.h
    src/unnecessary/math/rect.cpp

    DEPENDENCIES
    unnecessary_core
    glm::glm

    TESTS
    tests/rects.cpp
)
