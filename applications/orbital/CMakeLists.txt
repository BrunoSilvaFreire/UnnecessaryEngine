add_executable(orbital orbital.cpp)
target_link_libraries(orbital unnecessary)
includeShader(
    orbital
    ../../engine/resources/shaders/glsl/phong/phong.frag.glsl
)
includeShader(
    orbital
    ../../engine/resources/shaders/glsl/phong/phong.vert.glsl
)
includeResource(
    orbital
    ../../engine/resources/models/teapot_normal.fbx
)