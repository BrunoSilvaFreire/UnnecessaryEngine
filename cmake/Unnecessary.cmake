function(add_unnecessary_thirdparty_library NAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/${NAME})
    message("Added ${CMAKE_SOURCE_DIR}/thirdparty/${NAME}")
endfunction()

function(add_shader SHADER)
    get_filename_component(shader_filename ${SHADER} NAME)
    set(shader_abs ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    set(target_name compileUnnecessaryShader-${shader_filename})
    string(REGEX MATCHALL "([a-zA-Z]+).([a-zA-Z]+).([a-zA-Z]+)" SHADER_TYPE "${shader_filename}")
    set(SHADER_TYPE ${CMAKE_MATCH_2})
    set(shader_output ${shader_output_dir}/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.spv)
    message("${shader_abs}")
    set_source_files_properties("${shader_abs}" PROPERTIES VS_TOOL_OVERRIDE "None")
    get_source_file_property("${shader_abs}" found VS_TOOL_OVERRIDE)
    message("${found}")
    add_custom_target(
            ${target_name}
            COMMENT "Compiling ${shader_abs} to SPIR_V (${shader_output})"
            BYPRODUCTS ${shader_output}
            SOURCES "${shader_abs}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND glslc -fshader-stage=${SHADER_TYPE} ${shader_abs} -o ${shader_output}
    )
    set(shader_output_asm ${shader_output_dir}/${shader_filename}.spvasm)

    add_custom_target(
            "assembleUnnecessaryShader-${shader_filename}"
            COMMENT "Assembling ${shader_abs} to SPIR_V (${shader_output_asm})"
            BYPRODUCTS ${shader_output}
            SOURCES "${shader_abs}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND glslc -fshader-stage=${SHADER_TYPE} ${shader_abs} -S -g -o ${shader_output_asm}
    )
    set(shader_output_glsl ${shader_output_dir}/${shader_filename}.glsl)
    add_custom_target(
            "convertUnnecessaryShaderToGlsl-${shader_filename}"
            COMMENT "Convert SPIR-V ${shader_abs} to GLSL (${shader_output_asm})"
            DEPENDS ${target_name}
            SOURCES "${shader_abs}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND spirv-cross ${shader_output} -V --output ${shader_output_glsl}
    )
    add_dependencies(unnecessary ${target_name})
endfunction()