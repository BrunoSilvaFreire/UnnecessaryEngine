#include <gtest/gtest.h>
#include <filesystem>
#include <packer.h>

TEST(packer_tool, pack_random_images) {
    std::vector<std::string> files;
    std::filesystem::path imagesDir = "images";
    GTEST_LOG_(INFO) << "Looking for images in " << std::filesystem::absolute(imagesDir);
    std::filesystem::directory_iterator iterator(imagesDir);
    for (auto entry : iterator) {
        files.emplace_back(entry.path().string());
    }
    un::packer::pack(files);
}