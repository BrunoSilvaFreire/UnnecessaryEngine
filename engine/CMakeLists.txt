find_package(Vulkan REQUIRED)
add_executable(
        unnecessary
        include/unnecessary/application.h
        include/unnecessary/systems/world.h
        src/unnecessary/world.cpp
        src/unnecessary/dummy.cpp
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
        include/unnecessary/graphics/buffer.h
        src/unnecessary/graphics/buffer.cpp
        include/unnecessary/graphics/renderer.h
        include/unnecessary/algorithm/requirements.h
        src/unnecessary/algorithm/requirements.cpp
        src/unnecessary/algorithm/vulkan_requirements.cpp
        include/unnecessary/algorithm/vulkan_requirements.h
        include/unnecessary/components/common.h
        include/unnecessary/systems/transform.h
        include/unnecessary/systems/system.h
        src/unnecessary/systems/transform.cpp
        include/unnecessary/algorithm/event.h
        include/unnecessary/graphics/shader_stage.h
        src/unnecessary/graphics/shader_stage.cpp
        include/unnecessary/graphics/shader.h
        src/unnecessary/graphics/shader.cpp
        include/unnecessary/algorithm/layout.h
        include/unnecessary/graphics/systems/rendering.h
        include/unnecessary/graphics/graphics_pipeline.h
        src/unnecessary/graphics/graphics_pipeline.cpp
        include/unnecessary/graphics/queue.h
        src/unnecessary/graphics/queue.cpp
        include/unnecessary/render_target.h
        include/unnecessary/graphics/material.h
        src/unnecessary/graphics/material.cpp
        include/unnecessary/graphics/vertex_layout.h
        src/unnecessary/graphics/vertex_layout.cpp
        include/unnecessary/graphics/matrices.h
        include/unnecessary/graphics/descriptor_layout.h
        src/unnecessary/graphics/descriptor_layout.cpp
        include/unnecessary/graphics/drawable.h
        src/unnecessary/graphics/systems/rendering.cpp
        src/unnecessary/jobs/loading_jobs.cpp
        include/unnecessary/jobs/loading_jobs.h
        src/unnecessary/jobs/jobs.cpp
        src/unnecessary/def.cpp
        include/unnecessary/systems/run_system_job.h
        src/unnecessary/systems/run_system_job.cpp
        include/unnecessary/systems/parallel_system_data.h
        src/unnecessary/systems/parallel_system_data.cpp
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
)
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
        SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}
)
set(
        SHADERS
        resources/shaders/standart.frag
        resources/shaders/standart.vert
)
set(resources_output ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
set(shader_output_dir ${resources_output}/shaders)
foreach (shader ${SHADERS})
    add_shader(${shader})
endforeach ()
set(
        RESOURCES
        ../resources/teapot.obj
)

foreach (resource ${RESOURCES})
    get_filename_component(resource_filename ${resource} NAME)
    set(resource_abs ${CMAKE_SOURCE_DIR}/resources/${resource_filename})
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
        grapphs
)