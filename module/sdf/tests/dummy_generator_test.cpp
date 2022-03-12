#include <gtest/gtest.h>
#include <filesystem>
#include <unnecessary/sdf/generator/sdf_generator.h>


TEST(sdf_generator, dummy_generation) {
    std::filesystem::path image = "tests/dummy_image.png";
    std::filesystem::path output = image;
    output.replace_extension(".sdf" + image.extension().string());
    image = std::filesystem::absolute(image);
    output = std::filesystem::absolute(output);
    LOG(INFO) << "Testing sdf image " << image;
    int outputSize = 256;
    float min = 0;
    float max = 64;
    un::sdf::process_sdf(
        outputSize,
        24,
        image.string(),
        output.string(),
        min,
        max
    );
}