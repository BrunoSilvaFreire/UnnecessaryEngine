add_unnecessary_module(
    unnecessary_atlas

    SOURCES
    include/unnecessary/atlas/atlas.h
    include/unnecessary/atlas/atlas_serialization.h
    src/unnecessary/atlas/atlas_serialization.cpp

    DEPENDENCIES
    glm::glm
    unnecessary_jobs
    unnecessary_serialization
)

add_unnecessary_tool(
    unnecessary_atlas_packer

    SOURCES
    tools/packer.h
    tools/packer.cpp
    tools/image_rect.h
    tools/image_rect.cpp

    DEPENDENCIES
    unnecessary_atlas
    pngpp::pngpp

    TESTS
    tests/packer_tests.cpp
)
set(IMG_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/images)
message(${IMG_OUT_DIR})
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