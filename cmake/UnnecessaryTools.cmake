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
    )
    LIST(LENGTH UN_MODULE_DEPENDENCIES LISTCOUNT)
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${LISTCOUNT}" GREATER 0)
        target_link_libraries(${NAME} PUBLIC "${UN_MODULE_DEPENDENCIES}")
    endif ()
    target_compile_definitions(
            ${NAME}
            PUBLIC
            NOMINMAX
            VULKAN_HPP_TYPESAFE_CONVERSION
            VULKAN_HPP_NO_SPACESHIP_OPERATOR
    )
    LIST(LENGTH UN_MODULE_TESTS NUMTESTS)
    if ("${NUMTESTS}" GREATER 0)
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_TESTS})
        list(APPEND TEST_ALL_SOURCES ${UN_MODULE_SOURCES})
        add_executable(
            ${NAME}_tests
            ${TEST_ALL_SOURCES}
        )
        message("All: ${TEST_ALL_SOURCES}")
        target_compile_definitions(
            ${NAME}_tests
            PUBLIC
            DEBUG
            NOMINMAX
            UN_TESTING
            VULKAN_HPP_TYPESAFE_CONVERSION
            VULKAN_HPP_NO_SPACESHIP_OPERATOR
            SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}
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
        set_target_properties(
            ${NAME}_tests
            PROPERTIES
            LINKER_LANGUAGE CXX
        )
    endif ()
endfunction()