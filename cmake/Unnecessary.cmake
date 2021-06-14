function(add_unnecessary_thirdparty_library NAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/${NAME})
    message("Added ${CMAKE_SOURCE_DIR}/thirdparty/${NAME}")
endfunction()

function(add_shader SHADER)
    get_filename_component(shader_filename ${SHADER} NAME)
    set(shader_abs ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
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
endfunction()