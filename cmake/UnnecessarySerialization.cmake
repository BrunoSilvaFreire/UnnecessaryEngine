function(
    add_unnecessary_serialization
)
    set(FLAGS)
    set(
        SINGLE_VALUES
        TARGET
        RELATIVE_TO
        GLOBAL_INCLUDES
        FILES
    )
    set(
        MULTI_VALUES

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
    if (NOT UN_SERIALIZATION_RELATIVE_TO)
        set(UN_SERIALIZATION_RELATIVE_TO ${TGT_SRC}/include)
    endif ()
    set(SERIALIZER_INCLUDE_DIR ${SERIALIZER_OUTPUT}/include)
    foreach (include ${UN_SERIALIZATION_GLOBAL_INCLUDES})
        list(APPEND COMMAND_INCLUDES --global_include ${include})
    endforeach ()
    foreach (include ${TGT_INCLUDES})
        if (NOT include STREQUAL ${SERIALIZER_INCLUDE_DIR})
            list(APPEND COMMAND_INCLUDES --include ${include})
        endif ()
    endforeach ()

    foreach (file ${UN_SERIALIZATION_FILES})
        set(
            FILE_SERIALIZER_GENERATION_CMD
            unnecessary_serializer_generator
            ${TGT_SRC}/${file}
            ${COMMAND_INCLUDES}
            --output ${SERIALIZER_OUTPUT}
            --relative_to ${UN_SERIALIZATION_RELATIVE_TO}
        )
        list(JOIN FILE_SERIALIZER_GENERATION_CMD " " FILE_SERIALIZER_GENERATION_CMD_STR)
        message("File ${file} serialization using command '${FILE_SERIALIZER_GENERATION_CMD_STR}'")
        add_custom_target(
            _un_generate_${UN_SERIALIZATION_TARGET}_serialization
            COMMAND
            ${FILE_SERIALIZER_GENERATION_CMD}
            COMMENT "Generating serialization logic for ${UN_SERIALIZATION_TARGET}"
            SOURCES ${file}
        )
    endforeach ()
    add_dependencies(
        ${UN_SERIALIZATION_TARGET}
        _un_generate_${UN_SERIALIZATION_TARGET}_serialization
    )
    target_include_directories(
        ${UN_SERIALIZATION_TARGET}
        PUBLIC
        ${SERIALIZER_INCLUDE_DIR}
    )
endfunction()