set(
    CLANG_REQUIRED_TARGETS
    install-clangAST
    install-clangFrontend
    install-clangBasic
    install-clang-headers
    install-llvm-headers
    install-llvm-libraries
    install-libclang
    install-libclang-headers
)

set(LLVM_LOCAL_DIR ${CMAKE_SOURCE_DIR}/.cache/llvm)
set(LLVM_BUILDTYPE_DIR ${LLVM_LOCAL_DIR}/${CMAKE_BUILD_TYPE}/)
set(CLANG_GIT_DIRECTORY ${LLVM_LOCAL_DIR}/git)
set(CLANG_BUILD_DIRECTORY ${LLVM_BUILDTYPE_DIR}/build)
set(CLANG_INSTALL_DIRECTORY ${LLVM_BUILDTYPE_DIR}/install)

set(
    LLVM_LIBRARIES
    clangFrontend
    clangBasic
    LLVM-C
    LLVMAggressiveInstCombine
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitWriter
    LLVMBitstreamReader
    LLVMCFGuard
    LLVMCFIVerify
    LLVMCodeGen
    LLVMCore
    LLVMCoroutines
    LLVMCoverage
    LLVMDWARFLinker
    LLVMDWP
    LLVMDebugInfoCodeView
    LLVMDebugInfoDWARF
    LLVMDebugInfoGSYM
    LLVMDebugInfoMSF
    LLVMDebugInfoPDB
    LLVMDemangle
    LLVMDlltoolDriver
    LLVMExecutionEngine
    LLVMExegesis
    LLVMExegesisX86
    LLVMExtensions
    LLVMFileCheck
    LLVMFrontendOpenACC
    LLVMFrontendOpenMP
    LLVMFuzzMutate
    LLVMGlobalISel
    LLVMIRReader
    LLVMInstCombine
    LLVMInstrumentation
    LLVMInterfaceStub
    LLVMInterpreter
    LLVMJITLink
    LLVMLTO
    LLVMLibDriver
    LLVMLineEditor
    LLVMLinker
    LLVMMC
    LLVMMCA
    LLVMMCDisassembler
    LLVMMCJIT
    LLVMMCParser
    LLVMMIRParser
    LLVMObjCARCOpts
    LLVMObject
    LLVMObjectYAML
    LLVMOption
    LLVMOrcJIT
    LLVMOrcShared
    LLVMOrcTargetProcess
    LLVMPasses
    LLVMProfileData
    LLVMRemarks
    LLVMRuntimeDyld
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    LLVMSymbolize
    LLVMTableGen
    LLVMTableGenGlobalISel
    LLVMTarget
    LLVMTextAPI
    LLVMTransformUtils
    LLVMVectorize
    LLVMWindowsManifest
    LLVMX86AsmParser
    LLVMX86CodeGen
    LLVMX86Desc
    LLVMX86Disassembler
    LLVMX86Info
    LLVMXRay
    LLVMipo
)
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
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DLLVM_TARGETS_TO_BUILD=X86
        -DLLVM_ENABLE_PROJECTS=clang
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
    add_library(clangAST STATIC IMPORTED)
    add_library(libclang STATIC IMPORTED)
    target_include_directories(
        clangAST
        INTERFACE
        ${CLANG_INSTALL_DIRECTORY}/include
    )
    target_include_directories(
        libclang
        INTERFACE
        ${CLANG_INSTALL_DIRECTORY}/include
    )

    set_target_properties(
        clangAST
        PROPERTIES
        IMPORTED_LOCATION ${CLANG_INSTALL_DIRECTORY}/lib/clangAST.lib
        IMPORTED_CONFIGURATIONS Release
    )
    target_link_libraries(
        clangAST
        INTERFACE
        ${LLVM_ALL_LIBRARIES}
    )
    set_target_properties(
        libclang
        PROPERTIES
        IMPORTED_LOCATION ${CLANG_INSTALL_DIRECTORY}/lib/libclang.lib
        IMPORTED_CONFIGURATIONS Release
    )

    target_link_libraries(
        libclang
        INTERFACE
        ${LLVM_ALL_LIBRARIES}
    )
endmacro()