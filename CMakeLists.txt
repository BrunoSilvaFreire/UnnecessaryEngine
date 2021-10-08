project(UnnecessaryEngine LANGUAGES CXX)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)
include(CMakePrintHelpers)
include(cmake/Unnecessary.cmake)
conan_cmake_run(
        REQUIRES
        glm/0.9.9.8
        termcolor/1.0.1
        entt/3.8.1
        glfw/3.3.2
        tinyobjloader/1.0.6
        cxxopts/2.2.1

        SETTINGS
        compiler.cppstd=20

        BASIC_SETUP
        CMAKE_TARGETS
        BUILD missing
)


set(GRAPPHS_COMPILE_TESTS OFF CACHE BOOL "" FORCE)
add_unnecessary_thirdparty_library(grapphs)
add_subdirectory(engine)
add_subdirectory(applications/dummy)
add_compile_options(
        $<$<CXX_COMPILER_ID:MSVC>:/MP>
)
if (MINGW)
    message("Added options")
    add_compile_options(
            -static
            -static-libgcc
            -static-libstdc++
    )
    add_link_options(
            -static
            -static-libgcc
            -static-strc++
            -lwinpthread
            -lwsock32
            -lws2_32
    )
endif ()
