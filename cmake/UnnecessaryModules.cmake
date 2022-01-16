function(
    add_unnecessary_module
    NAME
)

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
    add_library(
        ${NAME}
        ${UN_MODULE_SOURCES}
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
        target_compile_definitions(
            ${NAME}_tests
            PUBLIC
            DEBUG
            NOMINMAX
            VULKAN_HPP_TYPESAFE_CONVERSION
            VULKAN_HPP_NO_SPACESHIP_OPERATOR
            SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}
        )
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