function(add_unnecessary_serialization)
    set(FLAGS)
    set(
        SINGLE_VALUES
        TARGET
        RELATIVE_TO
        GLOBAL_INCLUDES
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
    list(TRANSFORM UN_SERIALIZATION_FILES PREPEND ${TGT_SRC}/)
    foreach (file ${UN_SERIALIZATION_FILES})
        list(APPEND UN_SERIALIZATION_FILES_ARG --file ${file})
    endforeach ()
    list(
        APPEND
        SERIALIZATION_ARGS
        ${UN_SERIALIZATION_FILES_ARG}
        ${COMMAND_INCLUDES}
    )
    if (SERIALIZER_OUTPUT)
        list(
            APPEND
            SERIALIZATION_ARGS
            --output ${SERIALIZER_OUTPUT}

        )
    endif ()
    if (UN_SERIALIZATION_RELATIVE_TO)
        list(
            APPEND
            SERIALIZATION_ARGS
            --relative_to ${UN_SERIALIZATION_RELATIVE_TO}
        )
    endif ()
    set(
        FILE_SERIALIZER_GENERATION_CMD
        unnecessary_serializer_generator
        ${SERIALIZATION_ARGS}
    )
    list(JOIN FILE_SERIALIZER_GENERATION_CMD " " FILE_SERIALIZER_GENERATION_CMD_STR)
    message("Target ${UN_SERIALIZATION_TARGET} serialization using command '${FILE_SERIALIZER_GENERATION_CMD_STR}'")
    add_custom_target(
        generate_${UN_SERIALIZATION_TARGET}_serialization
        COMMAND "${FILE_SERIALIZER_GENERATION_CMD}"
        COMMENT "Generating serialization logic for ${UN_SERIALIZATION_TARGET}"
        SOURCES ${UN_SERIALIZATION_FILES}
    )
    add_dependencies(
        ${UN_SERIALIZATION_TARGET}
        generate_${UN_SERIALIZATION_TARGET}_serialization
    )
    target_include_directories(
        ${UN_SERIALIZATION_TARGET}
        PUBLIC
        ${SERIALIZER_INCLUDE_DIR}
    )
endfunction()