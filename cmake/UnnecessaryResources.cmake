function(
    unnecessary_copy_on_build
    TARGET
    FILE
    DESTINATION
)
    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMENT "Copying ${FILE} to ${DESTINATION}..."
        COMMAND ${CMAKE_COMMAND} -E copy "${FILE}" "${DESTINATION}"
        BYPRODUCTS ${DESTINATION}
    )
endfunction()

function(
    unnecessary_target_resources
)
    set(
        FLAGS
    )
    set(
        SINGLE_VALUES
        TARGET
    )
    set(
        MULTI_VALUES
        RESOURCES
    )
    cmake_parse_arguments(
        PARSE_ARGV 0
        UN_RESOURCE # Prefix
        # Options
        "${FLAGS}"
        "${SINGLE_VALUES}"
        "${MULTI_VALUES}"
        ${ARG_N}
    )
    foreach (RESOURCE ${UN_RESOURCE_RESOURCES})
        unnecessary_copy_on_build(
            ${UN_RESOURCE_TARGET}
            "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE}"
            ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE}
        )
    endforeach ()

endfunction()
