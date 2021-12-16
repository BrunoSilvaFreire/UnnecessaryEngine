

macro(get_unnecessary_shader_target_name SHADER OUTPUT)
    set(${OUTPUT} unnecessary_${SHADER}_shader)
endmacro()


function(add_unnecessary_shader PATH)
    file(
            READ ${PATH} SHADER_JSON
    )
    get_filename_component(PATH_ABS ${PATH} ABSOLUTE)

    get_filename_component(
            SHADER_JSON_DIRECTORY
            ${PATH}
            DIRECTORY
    )

    string(JSON SHADER_NAME GET "${SHADER_JSON}" name)
    get_unnecessary_shader_target_name(${SHADER_NAME} TARGET)
    add_custom_target(
            ${TARGET}
            COMMENT "Compiling shader '${SHADER_NAME}'"
    )
    get_target_property(TGT_DIR ${TARGET} BINARY_DIR)
    get_target_property(TGT_SRC_DIR ${TARGET} SOURCE_DIR)

    string(JSON SHADER_NUM_STAGES LENGTH "${SHADER_JSON}" stages)
    math(EXPR SHADER_STAGES_RANGE "${SHADER_NUM_STAGES}-1")
    foreach (STAGE_INDEX RANGE ${SHADER_STAGES_RANGE})
        string(JSON SHADER_STAGE_NAME MEMBER "${SHADER_JSON}" stages ${STAGE_INDEX})
        string(JSON SHADER_STAGE_FILE GET "${SHADER_JSON}" stages ${SHADER_STAGE_NAME})
        set(SHADER_STAGE_FILE "${TGT_SRC_DIR}/${SHADER_JSON_DIRECTORY}/${SHADER_NAME}.${SHADER_STAGE_NAME}.glsl")
        set(SHADER_OUTPUT_DIR "${TGT_DIR}/spirv/${SHADER_NAME}")
        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${SHADER_STAGE_NAME}.spirv")
        set(SHADER_REFLECTION_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${SHADER_STAGE_NAME}.reflection.json")
        add_custom_command(
                TARGET ${TARGET} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
        )
        add_custom_command(
                TARGET ${TARGET} PRE_BUILD
                COMMENT "Preprocessing stage '${SHADER_STAGE_NAME}' (${SHADER_OUTPUT})"
                MAIN_DEPENDENCY ${SHADER_STAGE_FILE}
                COMMAND
                spirv-cross ${SHADER_OUTPUT}
                --reflect
                --output ${SHADER_REFLECTION_OUTPUT}
                COMMAND
                unnecessary_shader_preprocessor
                ${PATH_ABS}
                --shader_stage ${SHADER_STAGE_NAME}
                --output ${SHADER_OUTPUT_DIR}
        )
        add_custom_command(
                TARGET ${TARGET} POST_BUILD
                BYPRODUCTS ${SHADER_OUTPUT}
                COMMENT "Compiling stage '${SHADER_STAGE_NAME}' (${SHADER_OUTPUT})"
#                MAIN_DEPENDENCY ${SHADER_STAGE_FILE}
                COMMAND
                glslc
                -fshader-stage=${SHADER_STAGE_NAME}
                ${SHADER_STAGE_FILE}
                -o ${SHADER_OUTPUT}
        )
    endforeach ()
endfunction()

function(include_unnecessary_shader TARGET SHADER)
    get_unnecessary_shader_target_name(${SHADER} SHADER_TARGET)
    add_dependencies(
            ${TARGET}
            ${SHADER_TARGET}
    )
    print_target_properties(${TARGET})
    print_target_properties(${SHADER_TARGET})

endfunction()