function(add_unnecessary_tool NAME)
    set(
            FLAGS
    )
    set(
            SINGLE_VALUES
    )
    set(
            MULTI_VALUES
            SOURCES
            DEPENDENCIES
            TESTS
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
    add_executable(
            ${NAME}
            ${UN_MODULE_SOURCES}
    )
    target_include_directories(
            ${NAME}
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/tools
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
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${NUMTESTS}" GREATER 0)
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_TESTS})
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_SOURCES})
        add_executable(
                ${NAME}_tests
                ${TEST_ALL_SOURCES}
        )
        message("All: ${TEST_ALL_SOURCES}")
        configure_unnecessary_module(${NAME}_tests)
        target_compile_definitions(
                ${NAME}_tests
                PUBLIC
                UN_TESTING
        )
        target_include_directories(
                ${NAME}_tests
                PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/tools
        )
        target_link_libraries(
                ${NAME}_tests
                PUBLIC
                GTest::GTest
                ${UN_MODULE_DEPENDENCIES}
        )
    endif ()
endfunction()