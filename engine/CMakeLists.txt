find_package(EnTT REQUIRED)
find_package(glfw3 REQUIRED)
find_package(termcolor REQUIRED)
find_package(Vulkan REQUIRED)
add_executable(
        unnecessary
        include/unnecessary/application.h
        include/unnecessary/world.h
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
        include/unnecessary/renderer.h
        include/unnecessary/algorithm/requirements.h
        src/unnecessary/algorithm/requirements.cpp
        src/unnecessary/algorithm/vulkan_requirements.cpp
        include/unnecessary/algorithm/vulkan_requirements.h include/unnecessary/components/common.h include/unnecessary/systems/transform.h include/unnecessary/systems/system.h src/unnecessary/systems/transform.cpp include/unnecessary/algorithm/event.h include/unnecessary/graphics/shader_stage.h src/unnecessary/graphics/shader_stage.cpp include/unnecessary/graphics/shader.h src/unnecessary/graphics/shader.cpp include/unnecessary/algorithm/layout.h include/unnecessary/systems/rendering.h include/unnecessary/graphics/pipeline_spec.h src/unnecessary/graphics/graphics_pipeline_spec.cpp include/unnecessary/graphics/frame_graph.h src/unnecessary/graphics/frame_graph.cpp)
target_include_directories(
        unnecessary
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_definitions(unnecessary PUBLIC DEBUG)
set(
        SHADERS
        resources/shaders/standart.frag
        resources/shaders/standart.vert
)
set(resources_output ${CMAKE_CURRENT_BINARY_DIR}/resources)
set(shader_output_dir ${resources_output}/shaders)
foreach (shader ${SHADERS})
    get_filename_component(shader_filename ${shader} NAME)
    set(shader_abs ${CMAKE_CURRENT_SOURCE_DIR}/${shader})
    set(shader_output ${shader_output_dir}/${shader_filename}.spv)
    set(target_name compileUnnecessaryShader-${shader_filename})
    add_custom_target(
            ${target_name}
            COMMENT "Compiling ${shader_abs} to SPIR_V (${shader_output})"
            BYPRODUCTS ${shader_output}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND glslc ${shader_abs} -o ${shader_output}

    )
    add_dependencies(unnecessary ${target_name})
endforeach ()

target_link_libraries(
        unnecessary
        EnTT::EnTT
        termcolor::termcolor
        glfw
        Vulkan::Vulkan
)