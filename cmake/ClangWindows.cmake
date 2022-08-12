set(
    CLANG_REQUIRED_TARGETS
    install-clangAST-stripped
    install-clangFrontend-stripped
    install-clangBasic-stripped
    install-libclang-stripped
    install-clang-stripped
    install-llvm-config-stripped

    install-clang-headers
    install-llvm-headers
    install-libclang-headers
)

set(LLVM_LOCAL_DIR ${CMAKE_SOURCE_DIR}/.cache/llvm)
set(LLVM_BUILDTYPE_DIR ${LLVM_LOCAL_DIR})
set(CLANG_GIT_DIRECTORY ${LLVM_LOCAL_DIR}/git)
set(CLANG_BUILD_DIRECTORY ${LLVM_BUILDTYPE_DIR}/build)
set(CLANG_INSTALL_DIRECTORY ${LLVM_BUILDTYPE_DIR}/install)
set(LLVM_DOWNLOAD_DIR ${CLANG_INSTALL_DIRECTORY})
list(
    TRANSFORM
    LLVM_LIBRARIES
    APPEND .lib
    OUTPUT_VARIABLE LLVM_ALL_LIBRARIES
)
list(
    TRANSFORM
    LLVM_ALL_LIBRARIES
    PREPEND ${CLANG_INSTALL_DIRECTORY}/lib/
)
set(
    CLANG_BRANCH
    release/13.x
)


macro(check_llvm_variable_or_copy VAR)
    if (NOT LLVM_${VAR})
        message("Auto-setting LLVM_${VAR} to ${CMAKE_${VAR}}")
        set(LLVM_${VAR} ${CMAKE_${VAR}})
    else ()
        message("LLVM CMake variable LLVM_${VAR} manually set to ${LLVM_${VAR}}")
    endif ()
endmacro()
check_llvm_variable_or_copy(C_COMPILER)
check_llvm_variable_or_copy(CXX_COMPILER)
check_llvm_variable_or_copy(MAKE_PROGRAM)
check_llvm_variable_or_copy(GENERATOR)

macro(clone_llvm)
    message("Cloning LLVM")
    execute_process(
        COMMAND
        git clone https://github.com/llvm/llvm-project
        --depth 1 --branch ${CLANG_BRANCH} ${CLANG_GIT_DIRECTORY}
    )
endmacro()

macro(build_llvm)
    execute_process(
        COMMAND
        ${CMAKE_COMMAND}
        -E make_directory
        ${CLANG_BUILD_DIRECTORY}
    )
    execute_process(
        COMMAND
        ${CMAKE_COMMAND}
        -G ${LLVM_GENERATOR}
        -DCMAKE_BUILD_TYPE=Release
        -DLLVM_TARGETS_TO_BUILD=X86
        -DLLVM_ENABLE_PROJECTS=clang
        -DLIBCLANG_BUILD_STATIC=On
        -DCMAKE_INSTALL_PREFIX=${CLANG_INSTALL_DIRECTORY}
        -DCMAKE_C_COMPILER=${LLVM_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${LLVM_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${LLVM_MAKE_PROGRAM}
        ${CLANG_GIT_DIRECTORY}/llvm
        WORKING_DIRECTORY ${CLANG_BUILD_DIRECTORY}
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CLANG_BUILD_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CLANG_INSTALL_DIRECTORY}
    )

    execute_process(
        COMMAND
        ${CMAKE_COMMAND}
        --build .
        --target ${CLANG_REQUIRED_TARGETS}
        WORKING_DIRECTORY ${CLANG_BUILD_DIRECTORY}
    )
endmacro()

macro(setup_llvm)
    if (NOT EXISTS ${CLANG_GIT_DIRECTORY})
        clone_llvm()
    endif ()
    if (NOT EXISTS ${CLANG_INSTALL_DIRECTORY})
        build_llvm()
    endif ()

    if (WIN32)
        add_library(libclang_s STATIC IMPORTED GLOBAL)
        set_target_properties(
            libclang_s
            PROPERTIES
            IMPORTED_LOCATION ${CLANG_INSTALL_DIRECTORY}/lib/libclang.lib
            IMPORTED_IMPLIB ${CLANG_INSTALL_DIRECTORY}/lib/libclang.lib
        )
    endif ()
endmacro()