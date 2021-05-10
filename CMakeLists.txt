project(UnnecessaryEngine LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(
        REQUIRES
        glm/0.9.9.8
        termcolor/1.0.1
        entt/3.5.2
        glfw/3.3.2
        tinyobjloader/1.0.6
        grapphs/0.1.0

        SETTINGS
        compiler.cppstd=20

        BASIC_SETUP
        CMAKE_TARGETS
        BUILD missing
)
add_subdirectory(engine)