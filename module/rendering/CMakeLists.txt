find_package(glfw3 REQUIRED)
add_unnecessary_module(
    unnecessary_rendering

    SOURCES

    include/unnecessary/rendering/renderer.h
    include/unnecessary/rendering/render_graph.h
    include/unnecessary/rendering/render_pass_description.h
    include/unnecessary/rendering/pipelines/inputs.h
    include/unnecessary/rendering/pipelines/pipeline.h
    include/unnecessary/rendering/pipelines/pipeline_stage.h

    src/unnecessary/rendering/renderer.cpp
    src/unnecessary/rendering/render_graph.cpp
    src/unnecessary/rendering/render_pass_description.cpp
    src/unnecessary/rendering/layout/layout.cpp
    src/unnecessary/rendering/layout/vertex_layout.cpp
    src/unnecessary/rendering/pipelines/inputs.cpp
    src/unnecessary/rendering/pipelines/pipeline.cpp
    src/unnecessary/rendering/pipelines/pipeline_stage.cpp


    DEPENDENCIES
    unnecessary_core
    unnecessary_jobs
    unnecessary_graphs
    glfw::glfw
)
add_unnecessary_tool(
    unnecessary_shader_tool

    SOURCES
    tools/unnecessary/shader_tool/unnecessary_shader_tool.cpp
    tools/unnecessary/shader_tool/shader_meta.h
    tools/unnecessary/shader_tool/shader_meta.cpp
    tools/unnecessary/shader_tool/shader_parsing.h
    tools/unnecessary/shader_tool/shader_parsing.cpp
    tools/unnecessary/shader_tool/inputs/glsl_type.h
    tools/unnecessary/shader_tool/inputs/glsl_type.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_rendering
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
)
add_unnecessary_shader(
    resources/shaders/phong/phong.shader.json
)

#[[
include_unnecessary_shader(
        unnecessary_rendering
        phong
)]]
