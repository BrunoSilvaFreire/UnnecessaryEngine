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
endfunction()