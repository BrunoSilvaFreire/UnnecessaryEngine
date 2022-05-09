function(
    add_unnecessary_serialization
)
    set(FLAGS)
    set(
        SINGLE_VALUES
        TARGET
    )
    set(
        MULTI_VALUES
        FILES
    )
    cmake_parse_arguments(
        PARSE_ARGV 0
        UN_SERIALIZATION # Prefix
        # Options
        "${FLAGS}"
        "${SINGLE_VALUES}"
        "${MULTI_VALUES}"
        ${ARG_N}
    )

    get_target_property(TGT_DIR ${UN_SERIALIZATION_TARGET} BINARY_DIR)
    set(SERIALIZER_OUTPUT "${TGT_DIR}/codegen")
    collect_unnecessary_includes(${UN_SERIALIZATION_TARGET} TGT_INCLUDES)
    get_target_property(TGT_SRC ${UN_SERIALIZATION_TARGET} SOURCE_DIR)

    set(SERIALIZER_INCLUDE_DIR ${SERIALIZER_OUTPUT}/include)
    foreach (include ${TGT_INCLUDES})
        if (NOT include STREQUAL ${SERIALIZER_INCLUDE_DIR})
            list(APPEND COMMAND_INCLUDES --include ${include})
        endif ()
    endforeach ()

    foreach (file ${UN_SERIALIZATION_FILES})
        add_custom_command(
            TARGET ${UN_SERIALIZATION_TARGET}
            PRE_BUILD
            COMMAND
            unnecessary_serializer_generator
            ${TGT_SRC}/${file}
            ${COMMAND_INCLUDES}
            --output ${SERIALIZER_OUTPUT}
            --relative_to ${TGT_SRC}/include
        )
    endforeach ()
    target_include_directories(
        ${UN_SERIALIZATION_TARGET}
        PUBLIC
        ${SERIALIZER_INCLUDE_DIR}
    )
endfunction()