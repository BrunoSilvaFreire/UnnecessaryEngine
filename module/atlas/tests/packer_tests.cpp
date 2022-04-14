#include <gtest/gtest.h>
#include <filesystem>
#include <packer.h>

TEST(packer_tool, pack_random_images) {
    std::vector<std::string> files;
    std::filesystem::path imagesDir = "images";
    GTEST_LOG_(INFO) << "Looking for images in " << std::filesystem::absolute(imagesDir);
    std::filesystem::directory_iterator iterator(imagesDir);
    for (const auto& entry : iterator) {
        if (!entry.is_regular_file()) {
            continue;
        }
        files.emplace_back(entry.path().string());
    }
    un::packer::pack(files);
}