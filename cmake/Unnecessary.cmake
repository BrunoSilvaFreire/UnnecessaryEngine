function(add_unnecessary_thirdparty_library NAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/${NAME})
    message("Added ${CMAKE_SOURCE_DIR}/thirdparty/${NAME}")
endfunction()

function(add_shader SHADER)
    get_filename_component(shader_filename ${SHADER} NAME_WLE)
    set(shader_abs ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    string(REGEX MATCHALL "([a-zA-Z]+).([a-zA-Z]+)" SHADER_TYPE "${shader_filename}")
    set(SHADER_TYPE ${CMAKE_MATCH_2})
    set(shader_output ${shader_output_dir}/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.spv)
    set_source_files_properties("${SHADER}" PROPERTIES VS_TOOL_OVERRIDE "None")

    get_source_file_property("${shader_abs}" found VS_TOOL_OVERRIDE)
    set(shader_identifier ${shader_filename})
    set(target_name compileShader-${shader_identifier})
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
            "assembleShader-${shader_identifier}"
            COMMENT "Assembling ${shader_abs} to SPIR_V (${shader_output_asm})"
            BYPRODUCTS ${shader_output_asm}
            SOURCES "${shader_abs}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND glslc -fshader-stage=${SHADER_TYPE} ${shader_abs} -S -g -o ${shader_output_asm}
    )
    set(shader_output_glsl ${shader_output_dir}/${shader_filename}.glsl)
    add_custom_target(
            "convertShaderToGlsl-${shader_identifier}"
            COMMENT "Convert SPIR-V ${shader_abs} to GLSL (${shader_output_glsl})"
            DEPENDS ${target_name}
            SOURCES "${shader_abs}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
            COMMAND spirv-cross ${shader_output} -V --output ${shader_output_glsl}
    )
    set(shader_output_json ${shader_output_dir}/${shader_filename}.json)
    add_custom_target(
        "convertShaderToJson-${shader_identifier}"
        COMMENT "Convert SPIR-V ${shader_abs} to Json (${shader_output_json})"
        DEPENDS ${target_name}
        SOURCES "${shader_abs}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
        COMMAND spirv-cross ${shader_output} --reflect --dump-resources --output ${shader_output_json}
    )
    add_dependencies(unnecessary ${target_name})
endfunction()