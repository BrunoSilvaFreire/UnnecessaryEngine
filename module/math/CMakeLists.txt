add_unnecessary_module(
    unnecessary_math

    SOURCES
    include/unnecessary/math/rect.h
    src/unnecessary/math/rect.cpp

    DEPENDENCIES
    unnecessary_core
    glm::glm

    TESTS
    tests/rects.cpp
)
