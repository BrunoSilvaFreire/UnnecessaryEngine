#include <gtest/gtest.h>
#include <unnecessary/math/rect.h>

TEST(rects, clip) {
    const u32 cutoff = 10;
    u32 size = 100;
    un::rectu rect(0, 0, size, size);
    u32 excluded = size - cutoff - cutoff;
    u32 maxLimit = size - cutoff - 1;
    un::rectu inset(cutoff, cutoff, excluded, excluded);
    GTEST_LOG_(INFO) << "rect: " << rect;
    GTEST_LOG_(INFO) << "inset: " << inset;
    rect.clip(inset);
    EXPECT_EQ(rect.get_max_y(), maxLimit)
                    << rect << " @ " << rect.get_max_y() << ", " << excluded << ", " << maxLimit;
    EXPECT_EQ(rect.get_min_y(), cutoff) << rect << " @ " << rect.get_min_y() << ", " << cutoff;
    EXPECT_EQ(rect.get_min_x(), cutoff) << rect << " @ " << rect.get_min_x() << ", " << cutoff;
    EXPECT_EQ(rect.get_max_x(), maxLimit)
                    << rect << " @ " << rect.get_max_x() << ", " << excluded << ", " << maxLimit;
}

TEST(rects, contains) {
    un::rectu a(0, 0, 10, 10);
    un::rectu b(0, 0, 5, 5);
    EXPECT_TRUE(a.contains(b));
    EXPECT_FALSE(b.contains(a));
}

TEST(rects, intersects) {
    un::rectu a(0, 0, 10, 10);
    un::rectu b(5, 5, 10, 10);
    EXPECT_TRUE(a.intersects(b));
    EXPECT_FALSE(b.intersects(a));
}

TEST(rects, set_max_min) {
    u32 size = 100;
    un::rectu rect(0, 0, size, size);
    EXPECT_EQ(rect.get_max_y(), size - 1) << rect;
    EXPECT_EQ(rect.get_min_y(), 0) << rect;
    EXPECT_EQ(rect.get_max_x(), size - 1) << rect;
    EXPECT_EQ(rect.get_min_x(), 0) << rect;
    rect.set_max_x(149);
    EXPECT_EQ(rect.get_width(), 150);
    EXPECT_EQ(rect.get_min_x(), 0);
    EXPECT_EQ(rect.get_max_x(), 149);
    rect.set_min_x(90);
    EXPECT_EQ(rect.get_width(), 60);
    EXPECT_EQ(rect.get_min_x(), 90);
    EXPECT_EQ(rect.get_max_x(), 149);

    rect.set_max_y(149);
    EXPECT_EQ(rect.get_height(), 150);
    EXPECT_EQ(rect.get_min_y(), 0);
    EXPECT_EQ(rect.get_max_y(), 149);
    rect.set_min_y(90);
    EXPECT_EQ(rect.get_height(), 60);
    EXPECT_EQ(rect.get_min_y(), 90);
    EXPECT_EQ(rect.get_max_y(), 149);
}
