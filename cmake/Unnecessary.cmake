include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryModules.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryShader.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryDebug.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryTools.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryResources.cmake)
include(GNUInstallDirs)

find_program(GLSLC_EXEC glslc REQUIRED)
find_program(SPIRV_EXEC spirv-cross REQUIRED)

function(add_unnecessary_thirdparty_library NAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/${NAME})
    message("Added ${CMAKE_SOURCE_DIR}/thirdparty/${NAME}")
endfunction()

function(set_unnecessary_compile_definitions TARGET)
    if (APPLE)
        target_compile_definitions(
            ${TARGET}
            PUBLIC
            APPLE
        )
    endif ()
endfunction()

function(add_shader SHADER)
    get_filename_component(shader_filename ${SHADER} NAME_WLE)
    set(shader_abs ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    string(REGEX MATCHALL "([a-z_A-Z]+).([a-zA-Z]+)" SHADER_TYPE "${shader_filename}")
    set(SHADER_TYPE ${CMAKE_MATCH_2})
    set(shader_output_dir ${CMAKE_BINARY_DIR}/shaders)
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
        COMMAND ${GLSLC_EXEC} -fshader-stage=${SHADER_TYPE} ${shader_abs} -o ${shader_output}
    )
    set(shader_output_asm ${shader_output_dir}/${shader_filename}.spvasm)

    add_custom_target(
        "assembleShader-${shader_identifier}"
        COMMENT "Assembling ${shader_abs} to SPIR_V (${shader_output_asm})"
        BYPRODUCTS ${shader_output_asm}
        SOURCES "${shader_abs}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
        COMMAND ${GLSLC_EXEC} -fshader-stage=${SHADER_TYPE} ${shader_abs} -S -g -o ${shader_output_asm}
    )
    set(shader_output_glsl ${shader_output_dir}/${shader_filename}.glsl)
    add_custom_target(
        "convertShaderToGlsl-${shader_identifier}"
        COMMENT "Convert SPIR-V ${shader_abs} to GLSL (${shader_output_glsl})"
        DEPENDS ${target_name}
        SOURCES "${shader_abs}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
        COMMAND ${SPIRV_EXEC} ${shader_output} -V --output ${shader_output_glsl}
    )
    set(shader_output_json ${shader_output_dir}/${shader_filename}.json)
    add_custom_target(
        "convertShaderToJson-${shader_identifier}"
        COMMENT "Convert SPIR-V ${shader_abs} to Json (${shader_output_json})"
        DEPENDS ${target_name}
        SOURCES "${shader_abs}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_output_dir}
        COMMAND ${SPIRV_EXEC} ${shader_output} --reflect --dump-resources --output ${shader_output_json}
    )
    add_dependencies(unnecessary ${target_name})
endfunction()
function(includeShader TARGET SHADER)
    get_filename_component(shader_filename ${SHADER} NAME_WLE)
    if (NOT TARGET compileShader-${shader_filename})
        message("No shader called ${SHADER} found.")
        return()
    endif ()
    string(REGEX MATCHALL "([a-z_A-Z]+).([a-zA-Z]+)" SHADER_TYPE "${shader_filename}")
    set(SHADER_TYPE ${CMAKE_MATCH_2})
    set(shader_output_dir ${CMAKE_BINARY_DIR}/shaders)
    set(shader_output ${shader_output_dir}/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.spv)
    get_target_property(TGT_DIR ${TARGET} ARCHIVE_OUTPUT_DIRECTORY)
    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMENT "Copying compiled shader (${shader_output})"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TGT_DIR}/resources/shaders/
        COMMAND ${CMAKE_COMMAND}
        -E copy
        "${CMAKE_BINARY_DIR}/shaders/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.spv"
        "${TGT_DIR}/resources/shaders/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.spv"
    )
endfunction()

function(includeResource TARGET RESOURCE)
    get_filename_component(resource_filename ${RESOURCE} NAME)
    get_filename_component(full_file ${RESOURCE} ABSOLUTE)
    get_target_property(TGT_DIR ${TARGET} BINARY_DIR)
    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMENT "Copying resource (${resource_filename})"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TGT_DIR}/resources/
        COMMAND ${CMAKE_COMMAND} -E copy
        "${full_file}"
        "${TGT_DIR}/resources/${resource_filename}"
    )
endfunction()
