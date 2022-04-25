#include <gtest/gtest.h>
#include <unnecessary/math/rect.h>

TEST(rects, clip) {
    const u32 cutoff = 10;
    u32 size = 100;
    un::uRect rect(0, 0, size, size);
    u32 excluded = size - cutoff - cutoff;
    u32 maxLimit = size - cutoff - 1;
    un::uRect inset(cutoff, cutoff, excluded, excluded);
    GTEST_LOG_(INFO) << "rect: " << rect;
    GTEST_LOG_(INFO) << "inset: " << inset;
    rect.clip(inset);
    EXPECT_EQ(rect.getMaxY(), maxLimit) << rect << " @ " << rect.getMaxY() << ", " << excluded << ", " << maxLimit;
    EXPECT_EQ(rect.getMinY(), cutoff) << rect << " @ " << rect.getMinY() << ", " << cutoff;
    EXPECT_EQ(rect.getMinX(), cutoff) << rect << " @ " << rect.getMinX() << ", " << cutoff;
    EXPECT_EQ(rect.getMaxX(), maxLimit) << rect << " @ " << rect.getMaxX() << ", " << excluded << ", " << maxLimit;
}

TEST(rects, contains) {
    un::uRect a(0, 0, 10, 10);
    un::uRect b(0, 0, 5, 5);
    EXPECT_TRUE(a.contains(b));
    EXPECT_FALSE(b.contains(a));
}

TEST(rects, intersects) {
    un::uRect a(0, 0, 10, 10);
    un::uRect b(5, 5, 10, 10);
    EXPECT_TRUE(a.intersects(b));
    EXPECT_FALSE(b.intersects(a));
}

TEST(rects, set_max_min) {
    u32 size = 100;
    un::uRect rect(0, 0, size, size);
    EXPECT_EQ(rect.getMaxY(), size - 1) << rect;
    EXPECT_EQ(rect.getMinY(), 0) << rect;
    EXPECT_EQ(rect.getMaxX(), size - 1) << rect;
    EXPECT_EQ(rect.getMinX(), 0) << rect;
    rect.setMaxX(149);
    EXPECT_EQ(rect.getWidth(), 150);
    EXPECT_EQ(rect.getMinX(), 0);
    EXPECT_EQ(rect.getMaxX(), 149);
    rect.setMinX(90);
    EXPECT_EQ(rect.getWidth(), 60);
    EXPECT_EQ(rect.getMinX(), 90);
    EXPECT_EQ(rect.getMaxX(), 149);

    rect.setMaxY(149);
    EXPECT_EQ(rect.getHeight(), 150);
    EXPECT_EQ(rect.getMinY(), 0);
    EXPECT_EQ(rect.getMaxY(), 149);
    rect.setMinY(90);
    EXPECT_EQ(rect.getHeight(), 60);
    EXPECT_EQ(rect.getMinY(), 90);
    EXPECT_EQ(rect.getMaxY(), 149);

}