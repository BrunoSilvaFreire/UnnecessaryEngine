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
    target_link_libraries(
        ${NAME}
        PUBLIC
        cxxopts::cxxopts
    )
    LIST(LENGTH UN_MODULE_DEPENDENCIES NUM_DEPENDENCIES)
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${NUM_DEPENDENCIES}" GREATER 0)
        target_link_libraries(${NAME} PUBLIC "${UN_MODULE_DEPENDENCIES}")
    endif ()
    configure_unnecessary_module(${NAME})
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${NUMTESTS}" GREATER 0)
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_TESTS})
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_SOURCES})
        create_unnecessary_tests_for_target(
            ${NAME}
            "${TEST_ALL_SOURCES}"
        )
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
            cxxopts::cxxopts
            ${UN_MODULE_DEPENDENCIES}
        )
    endif ()
endfunction()