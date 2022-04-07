include(GoogleTest)
function(
        create_unnecessary_tests_for_target
        TARGET
        TEST_SOURCES
)
    set(TEST_TARGET_NAME ${TARGET}_tests)
    add_executable(
            ${TEST_TARGET_NAME}
            "${TEST_SOURCES}"
    )
    configure_unnecessary_module(${NAME}_tests)
    list(LENGTH TEST_SOURCES NUM_TESTS)
    message(STATUS "Target ${TARGET} has ${NUM_TESTS} test source files. (${TEST_SOURCES})")
    target_link_libraries(
            ${NAME}_tests
            PUBLIC
            GTest::GTest
    )
    get_target_property(UN_TARGET_TYPE ${NAME} TYPE)
    if (NOT UN_TARGET_TYPE STREQUAL "EXECUTABLE")
        target_link_libraries(
                ${NAME}_tests
                PUBLIC
                ${NAME}
        )
    endif ()
    set_target_properties(
            ${NAME}_tests
            PROPERTIES
            LINKER_LANGUAGE CXX
    )
    gtest_discover_tests(
            ${TEST_TARGET_NAME}
    )

endfunction()