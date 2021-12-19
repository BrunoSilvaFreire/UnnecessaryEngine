add_executable(orbital orbital.cpp)
target_link_libraries(
    orbital
    unnecessary_rendering
    unnecessary_rendering
)
include_unnecessary_shader(orbital phong)
includeResource(
    orbital
    ../../engine/resources/models/teapot_normal.fbx
)