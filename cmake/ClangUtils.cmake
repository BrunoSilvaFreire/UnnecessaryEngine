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
    target_compile_definitions(
        cppast
        PUBLIC
        CINDEX_NO_EXPORTS
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