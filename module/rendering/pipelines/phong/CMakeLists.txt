add_unnecessary_shader(
    resources/phong/phong.shader.json
)


add_unnecessary_module(
    unnecessary_phong_pipeline

    SOURCES
    include/unnecessary/rendering/phong/phong_rendering_pipeline.h
    src/unnecessary/rendering/phong/phong_rendering_pipeline.cpp

    DEPENDENCIES
    unnecessary_rendering
)
include_unnecessary_shader(
    unnecessary_phong_pipeline
    phong
)