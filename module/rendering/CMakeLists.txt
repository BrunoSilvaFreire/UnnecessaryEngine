find_package(glfw3 REQUIRED)
add_unnecessary_module(
        unnecessary_rendering

        SOURCES
        include/unnecessary/rendering/renderer.h
        include/unnecessary/rendering/render_graph.h
        include/unnecessary/rendering/render_pass_description.h

        DEPENDENCIES
        unnecessary_core
        unnecessary_jobs
        unnecessary_graphs
        glfw::glfw
)
add_unnecessary_tool(
        unnecessary_shader_preprocessor

        SOURCES
        tools/unnecessary_shader_preprocessor.cpp

        DEPENDENCIES
        unnecessary_core
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
)
add_unnecessary_shader(
        resources/shaders/phong/phong.shader.json
)
include_unnecessary_shader(
        unnecessary_rendering
        phong
)