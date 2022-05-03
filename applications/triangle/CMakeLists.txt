add_unnecessary_application(
    triangle_sample

    SOURCES
    triangle.cpp
    draw_triangle_pass.h
    draw_triangle_pass.cpp
)
target_link_libraries(
    triangle_sample
    unnecessary_rendering
    unnecessary_simulation
    unnecessary_basic_pipeline
)
include_unnecessary_shader(triangle_sample triangle)
