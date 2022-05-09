add_unnecessary_library(
    unnecessary_rendering

    MODULE graphics

    SOURCES
    include/unnecessary/rendering/renderer.h
    include/unnecessary/rendering/render_graph.h
    include/unnecessary/rendering/render_pass.h
    include/unnecessary/rendering/render_group.h
    include/unnecessary/rendering/image.h
    include/unnecessary/rendering/image_view.h
    include/unnecessary/rendering/swapchain.h
    include/unnecessary/rendering/attachment.h
    include/unnecessary/rendering/framebuffer.h
    include/unnecessary/rendering/debug/vulkan_debugger.h
    include/unnecessary/rendering/jobs/render_job.h
    include/unnecessary/rendering/jobs/graphics_worker.h
    include/unnecessary/rendering/jobs/render_graph_system.h
    include/unnecessary/rendering/buffers/command_buffer.h
    include/unnecessary/rendering/layout/layout.h
    include/unnecessary/rendering/layout/vertex_layout.h
    include/unnecessary/rendering/layout/descriptor_set_layout.h
    include/unnecessary/rendering/pipelines/inputs.h
    include/unnecessary/rendering/pipelines/pipeline_description.h
    include/unnecessary/rendering/pipelines/pipeline.h
    include/unnecessary/rendering/pipelines/pipeline_stage.h

    src/unnecessary/rendering/renderer.cpp
    src/unnecessary/rendering/render_graph.cpp
    src/unnecessary/rendering/render_pass.cpp
    src/unnecessary/rendering/render_group.cpp
    src/unnecessary/rendering/image.cpp
    src/unnecessary/rendering/image_view.cpp
    src/unnecessary/rendering/queue.cpp
    src/unnecessary/rendering/rendering_device.cpp
    src/unnecessary/rendering/swapchain.cpp
    src/unnecessary/rendering/material.cpp
    src/unnecessary/rendering/window.cpp
    src/unnecessary/rendering/framebuffer.cpp
    src/unnecessary/rendering/attachment.cpp
    src/unnecessary/rendering/jobs/graphics_worker.cpp
    src/unnecessary/rendering/jobs/render_graph_system.cpp
    src/unnecessary/rendering/jobs/render_thread.cpp
    src/unnecessary/rendering/debug/vulkan_debugger.cpp
    src/unnecessary/rendering/vulkan/vulkan_utils.cpp
    src/unnecessary/rendering/vulkan/vulkan_requirements.cpp
    src/unnecessary/rendering/buffers/command_buffer.cpp
    src/unnecessary/rendering/layout/layout.cpp
    src/unnecessary/rendering/layout/vertex_layout.cpp
    src/unnecessary/rendering/layout/descriptor_set_layout.cpp
    src/unnecessary/rendering/pipelines/inputs.cpp
    src/unnecessary/rendering/pipelines/pipeline.cpp
    src/unnecessary/rendering/pipelines/pipeline_stage.cpp
    src/unnecessary/rendering/pipelines/pipeline_description.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_jobs
    unnecessary_graphs
    unnecessary_simulation
    unnecessary_serialization
    unnecessary_application
    Vulkan::Vulkan
    glm::glm
    glfw::glfw
    grapphs_visualization
)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    target_compile_definitions(
        unnecessary_rendering
        PUBLIC
        UN_VULKAN_DEBUG
    )
endif ()
add_unnecessary_tool(
    unnecessary_shaderizer

    MODULE graphics

    SOURCES
    tools/unnecessary/shaderizer/unnecessary_shaderizer.cpp
    tools/unnecessary/shaderizer/shader_meta.h
    tools/unnecessary/shaderizer/shader_meta.cpp
    tools/unnecessary/shaderizer/shader_stage_meta.h
    tools/unnecessary/shaderizer/shader_stage_meta.cpp
    tools/unnecessary/shaderizer/shader_parsing.h
    tools/unnecessary/shaderizer/shader_parsing.cpp
    tools/unnecessary/shaderizer/inputs/glsl_type.h
    tools/unnecessary/shaderizer/inputs/glsl_type.cpp
    tools/unnecessary/shaderizer/inputs/shader_input.h
    tools/unnecessary/shaderizer/inputs/shader_input.cpp

    DEPENDENCIES
    unnecessary_core
    unnecessary_rendering
    unnecessary_simulation
    unnecessary_application
    unnecessary_serialization
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
)
add_unnecessary_serialization(
    TARGET unnecessary_rendering
    FILES
    include/unnecessary/rendering/pipelines/pipeline_description.h
)

add_unnecessary_shader(
    resources/shaders/triangle.shader.json
)
add_subdirectory(
    pipelines/phong
)
add_subdirectory(
    pipelines/basic
)