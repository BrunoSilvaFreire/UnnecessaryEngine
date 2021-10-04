find_package(Vulkan REQUIRED)
add_library(
    unnecessary
    STATIC
    include/unnecessary/application.h
    include/unnecessary/systems/world.h
    src/unnecessary/world.cpp
    include/unnecessary/version.h
    include/unnecessary/def.h
    src/unnecessary/version.cpp
    include/unnecessary/strings.h
    src/unnecessary/application.cpp
    include/unnecessary/logging.h
    src/unnecessary/logging.cpp
    src/unnecessary/renderer.cpp
    include/unnecessary/graphics/mesh.h
    src/unnecessary/graphics/mesh.cpp
    include/unnecessary/graphics/buffers/buffer.h
    src/unnecessary/graphics/buffers/buffer.cpp
    include/unnecessary/graphics/renderer.h
    include/unnecessary/algorithm/requirements.h
    src/unnecessary/algorithm/requirements.cpp
    src/unnecessary/algorithm/vulkan_requirements.cpp
    include/unnecessary/algorithm/vulkan_requirements.h
    include/unnecessary/components/common.h
    include/unnecessary/systems/transform.h
    include/unnecessary/systems/system.h
    src/unnecessary/systems/system.cpp
    src/unnecessary/systems/transform.cpp
    include/unnecessary/algorithm/event.h
    include/unnecessary/graphics/shading/shader_stage.h
    src/unnecessary/graphics/shading/shader_stage.cpp
    include/unnecessary/graphics/shading/shader.h
    src/unnecessary/graphics/shading/shader.cpp
    include/unnecessary/algorithm/layout.h
    include/unnecessary/graphics/systems/projection.h
    include/unnecessary/graphics/pipeline/graphics_pipeline.h
    src/unnecessary/graphics/pipeline/graphics_pipeline.cpp
    include/unnecessary/graphics/queue.h
    src/unnecessary/graphics/queue.cpp
    include/unnecessary/render_target.h
    include/unnecessary/graphics/material.h
    src/unnecessary/graphics/material.cpp
    include/unnecessary/graphics/vertex_layout.h
    src/unnecessary/graphics/vertex_layout.cpp
    include/unnecessary/graphics/matrices.h
    include/unnecessary/graphics/descriptors/descriptor_layout.h
    src/unnecessary/graphics/descriptors/descriptor_layout.cpp
    include/unnecessary/graphics/drawable.h
    src/unnecessary/graphics/systems/projection.cpp
    src/unnecessary/jobs/loading_jobs.cpp
    include/unnecessary/jobs/loading_jobs.h
    src/unnecessary/jobs/jobs.cpp
    src/unnecessary/def.cpp
    include/unnecessary/systems/run_system_job.h
    src/unnecessary/systems/run_system_job.cpp
    include/unnecessary/graphics/systems/drawing.h
    src/unnecessary/graphics/systems/drawing.cpp
    include/unnecessary/graphics/disposable.h
    include/unnecessary/graphics/image.h
    src/unnecessary/graphics/image.cpp
    include/unnecessary/graphics/swapchain.h
    src/unnecessary/graphics/swapchain.cpp
    include/unnecessary/graphics/rendering_device.h
    src/unnecessary/graphics/rendering_device.cpp
    include/unnecessary/graphics/image_view.h
    src/unnecessary/graphics/image_view.cpp
    include/unnecessary/jobs/job_chain.h
    include/unnecessary/jobs/parallel_for_job.h
    src/unnecessary/jobs/parallel_for_job.cpp
    src/unnecessary/systems/velocity.cpp
    include/unnecessary/systems/velocity.h
    include/unnecessary/misc/benchmark.h
    src/unnecessary/misc/benchmark.cpp
    include/unnecessary/graphics/render_group.h
    include/unnecessary/graphics/descriptors/descriptor_allocator.h
    src/unnecessary/graphics/descriptors/descriptor_allocator.cpp
    include/unnecessary/systems/cameras.h
    src/unnecessary/systems/cameras.cpp
    include/unnecessary/graphics/lighting.h
    include/unnecessary/components/dummy.h
    include/unnecessary/platform.h
    src/unnecessary/platform.cpp
    include/unnecessary/systems/lighting.h
    src/unnecessary/systems/lighting.cpp
    include/unnecessary/graphics/pipeline/graphics_pipeline_layout.h
    src/unnecessary/graphics/pipeline/graphics_pipeline_layout.cpp
    include/unnecessary/graphics/descriptors/descriptor_set.h
    src/unnecessary/graphics/descriptors/descriptor_set.cpp
    include/unnecessary/graphics/buffers/buffer_writer.h
    src/unnecessary/graphics/buffers/buffer_writer.cpp
    include/unnecessary/graphics/buffers/command_buffer.h
    src/unnecessary/graphics/buffers/command_buffer.cpp
    src/unnecessary/jobs/job_chain.cpp
    include/unnecessary/misc/historic.h
    src/unnecessary/misc/historic.cpp
    include/unnecessary/misc/binary_writer.h
    src/unnecessary/misc/binary_writer.cpp
    include/unnecessary/systems/graphics_systems.h
    include/unnecessary/graphics/buffers/command_buffer_graph.h
    src/unnecessary/graphics/buffers/command_buffer_graph.cpp
    include/unnecessary/graphics/rendering/frame_graph.h
    src/unnecessary/graphics/frame_graph.cpp
    include/unnecessary/graphics/rendering/rendering_pipeline.h
    include/unnecessary/misc/aggregator.h
    src/unnecessary/systems/graphics_systems.cpp
    src/unnecessary/graphics/rendering_pipeline.cpp
    include/unnecessary/graphics/dispose_tracker.h
    src/unnecessary/graphics/dispose_tracker.cpp
    include/unnecessary/systems/simulation.h
    src/unnecessary/systems/simulation.cpp
    include/unnecessary/algorithm/dependency_graph.h
    include/unnecessary/misc/types.h
    src/unnecessary/misc/types.cpp
    include/unnecessary/algorithm/tree.h
    include/unnecessary/graphics/framebuffer.h
    include/unnecessary/graphics/rendering/renderpass.h
    src/unnecessary/graphics/rendering/renderpass.cpp
    include/unnecessary/graphics/rendering/baked_frame_graph.h
    src/unnecessary/graphics/rendering/baked_frame_graph.cpp
    src/unnecessary/graphics/framebuffer.cpp
    include/unnecessary/graphics/rendering/attachment.h
    src/unnecessary/graphics/rendering/attachment.cpp
    include/unnecessary/graphics/rendering/frame_resource.h
    src/unnecessary/graphics/rendering/frame_resource.cpp
    include/unnecessary/algorithm/vk_utility.h
    src/unnecessary/algorithm/vk_utility.cpp)

target_include_directories(
    unnecessary
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
message("${CMAKE_CURRENT_SOURCE_DIR}")
string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}_" SOURCE_PATH_SIZE)
target_compile_definitions(
    unnecessary
    PUBLIC
    DEBUG
    NOMINMAX
    VULKAN_HPP_TYPESAFE_CONVERSION
    VULKAN_HPP_NO_SPACESHIP_OPERATOR
    SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}
)
set(
    SHADERS
    resources/shaders/glsl/phong.frag.glsl
    resources/shaders/glsl/phong.vert.glsl
    resources/shaders/glsl/phong.geom.glsl
)
set(resources_output ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
set(shader_output_dir ${resources_output}/shaders)
foreach (shader ${SHADERS})
    add_shader(${shader})
endforeach ()
set(
    RESOURCES
    resources/models/teapot.obj
    resources/models/teapot_normal.obj
)

foreach (resource ${RESOURCES})
    get_filename_component(resource_filename ${resource} NAME)
    set(resource_abs "${CMAKE_CURRENT_SOURCE_DIR}/${resource}")
    set(resource_output ${resources_output}/${resource_filename})
    set(target_name copyResource-${resource_filename})
    add_custom_target(
        ${target_name}
        COMMENT "Copy resource ${resource_abs} to ${resource_output}"
        BYPRODUCTS ${resource_output}
        COMMAND ${CMAKE_COMMAND} -E copy ${resource_abs} ${resource_output}
    )
    add_dependencies(unnecessary ${target_name})
endforeach ()
target_link_libraries(
    unnecessary
    Vulkan::Vulkan
    CONAN_PKG::glm
    CONAN_PKG::entt
    CONAN_PKG::termcolor
    CONAN_PKG::glfw
    CONAN_PKG::tinyobjloader
    CONAN_PKG::cxxopts
    grapphs
)
if (WIN32)
endif ()
