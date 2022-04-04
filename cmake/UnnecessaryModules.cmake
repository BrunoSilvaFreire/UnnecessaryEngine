function(configure_unnecessary_module NAME)
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
    set_target_properties(
            ${NAME}
            PROPERTIES
            LINKER_LANGUAGE CXX
    )
endfunction()


function(
        add_unnecessary_module
        NAME
)

    set(FLAGS)
    set(SINGLE_VALUES)
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
    configure_unnecessary_module(${NAME})
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_definitions(
                ${NAME}
                PUBLIC
                DEBUG
        )
    endif ()
    if ("${NUMTESTS}" GREATER 0)
        add_executable(
                ${NAME}_tests
                ${UN_MODULE_TESTS}
        )
        configure_unnecessary_module(${NAME}_tests)

        target_link_libraries(
                ${NAME}_tests
                PUBLIC
                ${NAME}
                GTest::GTest
        )
        set_target_properties(
                ${NAME}_tests
                PROPERTIES
                LINKER_LANGUAGE CXX
        )
    endif ()
endfunction()