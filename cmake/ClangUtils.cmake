if (WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/ClangWindows.cmake)
elseif (APPLE)
    include(${CMAKE_SOURCE_DIR}/cmake/ClangApple.cmake)
else ()
    include(${CMAKE_SOURCE_DIR}/cmake/ClangUnix.cmake)
endif ()
function(setup_libast)
    setup_llvm()
    add_unnecessary_thirdparty_library(cppast)

    # I don't know why tpl adds that macro, but it causes problems, so away it goes
    get_target_property(DEFS cppast COMPILE_DEFINITIONS)
    message("CPPAST DEFS: ${DEFS}")
    list(REMOVE_ITEM DEFS -pendantic-errors)
    list(APPEND DEFS CINDEX_NO_EXPORTS)
    message("CPPAST DEFS: ${DEFS}")
    set_target_properties(
        cppast
        PROPERTIES
        COMPILE_DEFINITIONS "${DEFS}"
    )
    # I don't know why tpl adds that macro, but it causes problems, so away it goes
    get_target_property(DEFS tiny-process-library COMPILE_DEFINITIONS)
    list(REMOVE_ITEM DEFS /D_CRT_SECURE_NO_WARNINGS)
    set_target_properties(
        tiny-process-library
        PROPERTIES
        COMPILE_DEFINITIONS "${DEFS}"
    )

endfunction()