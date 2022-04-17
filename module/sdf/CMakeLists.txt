
#[[
add_unnecessary_module(
    unnecessary_sdf

    SOURCES

    DEPENDENCIES
    unnecessary_core
)
]]

add_unnecessary_tool(
    unnecessary_sdf_generator

    SOURCES
    tools/unnecessary/sdf/generator/sdf_generator.h
    tools/unnecessary/sdf/generator/sdf_generator.cpp
    tools/unnecessary/sdf/generator/jobs/process_pixel_job.h
    tools/unnecessary/sdf/generator/jobs/process_pixel_job.cpp

    DEPENDENCIES
    unnecessary_jobs
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
    pngpp::pngpp
    glm::glm
    Freetype::Freetype

    TESTS
    tests/dummy_generator_test.cpp
)
unnecessary_target_resources(
    TARGET unnecessary_sdf_generator_tests
    RESOURCES
    tests/dummy_image.png
)
