add_executable(orbital orbital.cpp)
target_link_libraries(
    orbital
    unnecessary_rendering
    unnecessary_phong_pipeline
    unnecessary_simulation
)
include_unnecessary_shader(orbital phong)
include_unnecessary_shader(orbital triangle)
includeResource(
    orbital
    ../../legacy/resources/models/teapot_normal.fbx
)