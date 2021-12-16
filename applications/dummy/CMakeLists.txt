add_executable(dummy dummy.cpp gameplay.cpp gameplay.h operations.h operations.cpp)
target_link_libraries(dummy unnecessary)

add_executable(dummy-headless dummy.cpp gameplay.cpp gameplay.h operations.h operations.cpp)
target_link_libraries(dummy-headless unnecessary)
target_compile_definitions(
    dummy-headless
    PUBLIC
    -DHEADLESS=1
)