cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(
    UnnecessaryEngine
    LANGUAGES CXX
    VERSION 0.0.1
)


include(cmake/Unnecessary.cmake)
find_package(Python REQUIRED)

option(UN_DISALLOW_AGGRESSIVE_INLINING DEFAULT OFF)

conan_cmake_configure(
    REQUIRES
    glm/0.9.9.8
    termcolor/2.0.0
    entt/3.9.0
    glfw/3.3.7
    tinyobjloader/1.0.6
    cxxopts/3.0.0
    gtest/1.11.0
    nlohmann_json/3.10.4
    pngpp/0.2.10
    tinygltf/2.5.0
    freetype/2.11.1
    stduuid/1.2.2
    zlib/1.2.12
    stb/20200203
    #    grpc/1.48.0

    GENERATORS
    cmake_find_package

    OPTIONS
    stduuid:with_cxx20_span=True
    freetype:with_brotli=False
    tinygltf:stb_image=False
    assimp:with_opengex=False
)

unnecessary_get_conan_settings(settings)

message("Conan cmake settings: ${settings}")
conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    SETTINGS ${settings}
    ENV
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    CONAN_DISABLE_CHECK_COMPILER=On
)
list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_BINARY_DIR}"
)
find_package(glm REQUIRED)
find_package(EnTT REQUIRED)
find_package(termcolor REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(cxxopts REQUIRED)
find_package(GTest REQUIRED)
find_package(stduuid REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(TinyGLTF REQUIRED)
target_compile_definitions(
    TinyGLTF::TinyGLTF
    INTERFACE
    TINYGLTF_NO_STB_IMAGE=1
    TINYGLTF_NO_STB_IMAGE_WRITE=1
    TINYGLTF_NO_INCLUDE_STB_IMAGE=1
    TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE=1
    TINYGLTF_NO_EXTERNAL_IMAGE=1
    TINYGLTF_USE_CPP14=1
)
find_package(pngpp REQUIRED)

set(GRAPPHS_COMPILE_TESTS OFF CACHE BOOL "" FORCE)
set(GRAPPHS_COMPILE_SAMPLES OFF CACHE BOOL "" FORCE)

add_unnecessary_thirdparty_library(grapphs)
#add_subdirectory(engine)
#add_subdirectory(applications/dummy)
add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
)

if (MINGW)
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
elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif ()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    if ("${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "/EHa ${CMAKE_CXX_FLAGS}")
        message("Appending /EHa to compile flags because we're using clang-cl (${CMAKE_CXX_FLAGS})")
    endif ()

endif ()
add_subdirectory(module/core)
add_subdirectory(module/cli)
add_subdirectory(module/math)
add_subdirectory(module/graphs)
add_subdirectory(module/jobs)
add_subdirectory(module/application)
add_subdirectory(module/rendering)
add_subdirectory(module/simulation)
add_subdirectory(module/sdf)
add_subdirectory(module/serialization)
add_subdirectory(module/atlas)
add_subdirectory(module/baf)
add_subdirectory(module/source_analysis)


add_subdirectory(applications/orbital)
add_subdirectory(applications/triangle)
