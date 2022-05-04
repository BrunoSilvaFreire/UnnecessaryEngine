find_program(
    LLVM_LLD
    lld
    HINTS
    $ENV{LLVM_HOME}/bin
    REQUIRED
)
cmake_path(
    GET
    LLVM_LLD
    PARENT_PATH
    LLVM_ROOT
)
cmake_path(
    GET
    LLVM_ROOT
    PARENT_PATH
    LLVM_ROOT
)

find_file(
    LIB_CLANG

    NAMES
    libclang.a
    libclang.lib

    HINTS
    ${LLVM_ROOT}/lib

    REQUIRED
)
find_file(
    LIB_CLANG_INDEX

    NAMES
    Index.h

    HINTS
    ${LLVM_ROOT}/include/clang-c

    REQUIRED
)
cmake_path(
    GET
    LIB_CLANG_INDEX
    PARENT_PATH
    CLANGC_INCLUDE
)

cmake_path(
    GET
    CLANGC_INCLUDE
    PARENT_PATH
    CLANGC_INCLUDE
)

add_library(
    clang-c
    STATIC IMPORTED
)
target_include_directories(
    clang-c
    INTERFACE
    "${CLANGC_INCLUDE}"
)

set_target_properties(
    clang-c
    PROPERTIES
    IMPORTED_LOCATION "${LIB_CLANG}"
)

message("Found llvm  @ ${LLVM_ROOT}, ${LIB_CLANG}")
add_unnecessary_library(
    unnecessary_source_analysis

    SOURCES
    include/unnecessary/source_analysis/structures.h
    include/unnecessary/source_analysis/parsing.h
    include/unnecessary/source_analysis/parser.h
    src/unnecessary/source_analysis/structures.cpp
    src/unnecessary/source_analysis/parsing.cpp
    src/unnecessary/source_analysis/parser.cpp

    DEPENDENCIES
    unnecessary_core
    clang-c
)