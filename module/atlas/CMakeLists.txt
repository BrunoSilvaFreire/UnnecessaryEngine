add_unnecessary_module(
    unnecessary_atlas

    SOURCES
    include/unnecessary/atlas/atlas.h
    include/unnecessary/atlas/atlas_serialization.h
    src/unnecessary/atlas/atlas_serialization.cpp

    DEPENDENCIES
    unnecessary_math
    unnecessary_jobs
    unnecessary_serialization
)

add_unnecessary_tool(
    unnecessary_atlas_packer

    SOURCES
    tools/packer.h
    tools/packer.cpp
    tools/packer_entry.h
    tools/packer_entry.cpp
    tools/packing_algorithm.h
    tools/algorithms/max_rectangles_algorithm.h
    tools/algorithms/max_rectangles_algorithm.cpp
    tools/jobs/pack_texture_job.h
    tools/jobs/pack_texture_job.cpp

    DEPENDENCIES
    unnecessary_atlas
    pngpp::pngpp
    glm::glm
    grapphs_visualization

    TESTS
    tests/packer_tests.cpp
)
set(IMG_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/images)

add_custom_command(
    TARGET unnecessary_atlas_packer_tests
    POST_BUILD

    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${IMG_OUT_DIR}

    COMMAND ${Python_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/image_gen.py
    --num_images 32
    --reuse true
    --output ${IMG_OUT_DIR}
    BYPRODUCTS ${IMG_OUT_DIR}
)