include(${CMAKE_SOURCE_DIR}/cmake/UnnecessaryTests.cmake)

function(
    configure_unnecessary_target
    NAME
    SHOULD_INSTALL
)
    target_compile_definitions(
        ${NAME}
        PUBLIC
        NOMINMAX
        VULKAN_HPP_TYPESAFE_CONVERSION
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
    )
    if (UN_DISALLOW_AGGRESSIVE_INLINING)
        target_compile_definitions(
            ${NAME}
            PUBLIC
            UN_DISALLOW_AGGRESSIVE_INLINING=1
        )
    endif ()
    if (WIN32)
        target_compile_definitions(
            ${NAME}
            PUBLIC
            UN_PLATFORM_WINDOWS
        )
    elseif (UNIX)
        target_compile_definitions(
            ${NAME}
            PUBLIC
            UN_PLATFORM_UNIX
        )
    endif ()
    set_target_properties(
        ${NAME}
        PROPERTIES
        LINKER_LANGUAGE CXX
    )
    set_target_properties(
        ${NAME}
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 20
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
    get_target_property(MODULE_SRC ${NAME} SOURCES)
    get_target_property(
        MODULE
        ${NAME}
        UNNECESSARY_MODULE
    )
    foreach (src ${MODULE_SRC})
        if (NOT ${MODULE} STREQUAL "MODULE-NOTFOUND")
            string(
                APPEND
                src
                "${MODULE}:"
            )
        endif ()
        set_source_files_properties(
            ${src}
            PROPERTIES
            COMPILE_DEFINITIONS "UN_CMAKE_FILE_NAME=\"${src}\""
        )
    endforeach ()
    if (SHOULD_INSTALL)

        if (MODULE STREQUAL "MODULE-NOTFOUND")
            set(MODULE "miscellaneous")
        endif ()
        install(
            TARGETS ${NAME}
            EXPORT ${NAME}
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/${MODULE}/bin
            ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/modules/${MODULE}/lib
        )
        get_target_property(target_type ${NAME} TYPE)
        if (NOT target_type STREQUAL EXECUTABLE)
            get_target_property(
                INC_DIR
                ${NAME}
                INTERFACE_INCLUDE_DIRECTORIES
            )
            install(
                DIRECTORY ${INC_DIR}
                DESTINATION modules/${MODULE}
            )
        endif ()

    endif ()

endfunction()


function(
    add_unnecessary_library
    NAME
)

    set(FLAGS)
    set(
        SINGLE_VALUES
        MODULE
    )
    set(
        MULTI_VALUES
        SOURCES
        DEPENDENCIES
        TESTS
        SOURCES_APPLE
        SOURCES_WIN
        SOURCES_LINUX
    )
    cmake_parse_arguments(
        PARSE_ARGV 0
        UN_MODULE # Prefix
        # Options
        "${FLAGS}"
        "${SINGLE_VALUES}"
        "${MULTI_VALUES}"
        ${ARG_N}
    )
    set(ALL_SOURCES ${UN_MODULE_SOURCES})

    if (APPLE)
        list(APPEND ALL_SOURCES ${UN_MODULE_SOURCES_APPLE})
    elseif (WIN32)
        list(APPEND ALL_SOURCES ${UN_MODULE_SOURCES_WIN})
    elseif (UNIX)
        list(APPEND ALL_SOURCES ${UN_MODULE_SOURCES_LINUX})
    endif ()
    add_library(
        ${NAME}
        ${ALL_SOURCES}
    )
    if (NOT ${UN_MODULE_MODULE} STREQUAL "")
        set_target_properties(
            ${NAME}
            PROPERTIES
            UNNECESSARY_MODULE ${UN_MODULE_MODULE}
        )
    endif ()
    target_include_directories(
        ${NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    set_target_properties(
        ${NAME}
        PROPERTIES
        LINKER_LANGUAGE CXX
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
    LIST(LENGTH UN_MODULE_DEPENDENCIES LISTCOUNT)
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${LISTCOUNT}" GREATER 0)
        target_link_libraries(${NAME} PUBLIC "${UN_MODULE_DEPENDENCIES}")
    endif ()
    configure_unnecessary_target(${NAME} TRUE)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_definitions(
            ${NAME}
            PUBLIC
            DEBUG
        )
    endif ()
    if ("${NUMTESTS}" GREATER 0)
        create_unnecessary_tests_for_target(${NAME} "${UN_MODULE_TESTS}")
    endif ()
endfunction()