#include <gtest/gtest.h>
#include <filesystem>
#include <packer.h>

void pack(
    const std::filesystem::path& imagesDir
) {
    std::vector<std::string> files;
    std::filesystem::directory_iterator iterator(imagesDir);
    for (const auto& entry : iterator) {
        if (!entry.is_regular_file()) {
            continue;
        }
        files.emplace_back(entry.path().string());
    }
    GTEST_LOG_(INFO) << "Found " << files.size() << " images in " << absolute(imagesDir);
    std::string prefix = imagesDir.filename().string() + '_';
    un::packer::pack(
        un::packer::max_rectangles_algorithm<un::bottom_left_heuristic>(),
        files,
        imagesDir.parent_path() / (prefix + "bottom_left.png"));

    un::packer::pack(
        un::packer::max_rectangles_algorithm<un::best_area_fit_heuristic>(),
        files,
        imagesDir.parent_path() / (prefix + "best_area.png")
    );
    un::packer::pack(
        un::packer::max_rectangles_algorithm<un::best_short_side_fit_heuristic>(), files,
        imagesDir.parent_path() / (
            prefix + "short_side.png"
        ));
    un::packer::pack(
        un::packer::max_rectangles_algorithm<un::best_long_side_fit_heuristic>(), files,
        imagesDir.parent_path() / (prefix + "long_side.png"));
}

TEST(packer_tool, pack_uniform_images) {
    std::filesystem::path imagesDir = "images/packed";
    pack(imagesDir);
}

TEST(packer_tool, pack_random_images) {
    std::filesystem::path imagesDir = "images/random";
    pack(imagesDir);
}
