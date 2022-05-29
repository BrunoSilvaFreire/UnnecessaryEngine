macro(unnecessary_get_conan_settings OUT_VAR)
    if (WIN32)
        if (CMAKE_BUILD_TYPE STREQUAL Debug)
            set(RUNTIME MDd)
        else ()
            set(RUNTIME MD)
        endif ()
        set(
            ${OUT_VAR}
            "arch=x86_64";
            "build_type=${CMAKE_BUILD_TYPE}";
            "compiler=Visual Studio";
            "compiler.version=17"
            "compiler.runtime=${RUNTIME}"
            "compiler.cppstd=20"
        )
        set(CONAN_SETTINGS ${${OUT_VAR}})
    else ()
        conan_cmake_autodetect(${OUT_VAR})
    endif ()
endmacro()


if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()

set(CONAN_DISABLE_CHECK_COMPILER On)
include(${CMAKE_BINARY_DIR}/conan.cmake)