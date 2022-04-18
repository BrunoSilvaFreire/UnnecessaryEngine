

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
    list(APPEND SHADER_SOURCES ${PATH})
    string(JSON SHADER_NUM_STAGES LENGTH "${SHADER_JSON}" stages)
    math(EXPR SHADER_STAGES_RANGE "${SHADER_NUM_STAGES}-1")
    add_custom_target(
        ${TARGET}
        COMMENT "Compiling shader '${SHADER_NAME}'"
    )
    get_target_property(TGT_DIR ${TARGET} BINARY_DIR)
    get_target_property(TGT_SRC_DIR ${TARGET} SOURCE_DIR)
    get_target_property(RENDERING_SRC_DIR unnecessary_rendering SOURCE_DIR)

    foreach (STAGE_INDEX RANGE ${SHADER_STAGES_RANGE})
        string(JSON SHADER_STAGE_NAME MEMBER "${SHADER_JSON}" stages ${STAGE_INDEX})
        set(SHADER_STAGE_FILE "${TGT_SRC_DIR}/${SHADER_JSON_DIRECTORY}/${SHADER_NAME}.${SHADER_STAGE_NAME}.glsl")
        list(APPEND SHADER_SOURCES ${SHADER_STAGE_FILE})
        set(SHADER_OUTPUT_DIR "${TGT_DIR}/spirv/${SHADER_NAME}")
        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${SHADER_STAGE_NAME}.spirv")
        set(SHADER_REFLECTION_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.${SHADER_STAGE_NAME}.reflection.json")
        set_target_properties(
            ${TARGET}
            PROPERTIES
            SHADER_OUTPUT_DIR ${SHADER_OUTPUT_DIR}

        )
        add_custom_command(
            TARGET ${TARGET} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
        )
        add_custom_command(
            TARGET ${TARGET} PRE_BUILD
            COMMENT "Preprocessing stage '${SHADER_STAGE_NAME}' (${SHADER_OUTPUT})"
            MAIN_DEPENDENCY ${SHADER_STAGE_FILE}

            COMMAND
            unnecessary_shaderizer
            ${PATH_ABS}
            --glsl
            --output ${SHADER_OUTPUT_DIR}
        )
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            BYPRODUCTS ${SHADER_OUTPUT}
            COMMENT "Compiling stage '${SHADER_STAGE_NAME}' (${SHADER_OUTPUT})"
            COMMAND
            ${GLSLC_EXEC}
            -fshader-stage=${SHADER_STAGE_NAME}
            ${SHADER_STAGE_FILE}
            -I "${RENDERING_SRC_DIR}/resources/shaders"
            -I "${SHADER_OUTPUT_DIR}"
            -o ${SHADER_OUTPUT}
            COMMAND
            ${SPIRV_EXEC} ${SHADER_OUTPUT}
            --reflect
            --output ${SHADER_REFLECTION_OUTPUT}
        )
        add_custom_command(
            TARGET ${TARGET} PRE_BUILD
            COMMENT "Preprocessing stage '${SHADER_STAGE_NAME}' (${SHADER_OUTPUT})"
            MAIN_DEPENDENCY ${SHADER_STAGE_FILE}

            COMMAND
            unnecessary_shaderizer
            ${PATH_ABS}
            --cpp
            --output ${SHADER_OUTPUT_DIR}
        )
    endforeach ()
    set_target_properties(
        ${TARGET}
        PROPERTIES
        SOURCES "${SHADER_SOURCES}"
    )
endfunction()

function(include_unnecessary_shader TARGET SHADER)
    get_unnecessary_shader_target_name(${SHADER} SHADER_TARGET)
    add_dependencies(
        ${TARGET}
        ${SHADER_TARGET}
    )
    get_target_property(
        SHADER_OUTPUT_DIR
        ${SHADER_TARGET}
        SHADER_OUTPUT_DIR
    )
    target_include_directories(
        ${TARGET}
        PUBLIC
        ${SHADER_OUTPUT_DIR}
    )
endfunction()