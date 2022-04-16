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
    ASSERT_EQ(rect.getMaxY(), maxLimit) << rect << " @ " << rect.getMaxY() << ", " << excluded << ", " << maxLimit;
    ASSERT_EQ(rect.getMinY(), cutoff) << rect << " @ " << rect.getMinY() << ", " << cutoff;
    ASSERT_EQ(rect.getMinX(), cutoff) << rect << " @ " << rect.getMinX() << ", " << cutoff;
    ASSERT_EQ(rect.getMaxX(), maxLimit) << rect << " @ " << rect.getMaxX() << ", " << excluded << ", " << maxLimit;
}

TEST(rects, contains) {
    un::uRect a(0, 0, 10, 10);
    un::uRect b(0, 0, 5, 5);
    ASSERT_TRUE(a.contains(b));
    ASSERT_FALSE(b.contains(a));
}

TEST(rects, exclude) {
    const u32 cutoff = 10;
    const u32 size = 100;
    un::uRect rect(0, 0, size, size);
    u32 maxLimit = size - 1;
    un::uRect exclusion(cutoff, 0, size - cutoff, size);
    GTEST_LOG_(INFO) << "rect: " << rect;
    GTEST_LOG_(INFO) << "exclusion: " << exclusion;
    rect.exclude(exclusion);
    ASSERT_EQ(rect.getMaxY(), maxLimit) << rect;
    ASSERT_EQ(rect.getMinY(), 0) << rect;
    ASSERT_EQ(rect.getMinX(), cutoff) << rect;
    ASSERT_EQ(rect.getMaxX(), maxLimit) << rect;
}

TEST(rects, set_max_min) {
    u32 size = 100;
    un::uRect rect(0, 0, size, size);
    ASSERT_EQ(rect.getMaxY(), size - 1) << rect;
    ASSERT_EQ(rect.getMinY(), 0) << rect;
    ASSERT_EQ(rect.getMaxX(), size - 1) << rect;
    ASSERT_EQ(rect.getMinX(), 0) << rect;
    rect.setMaxX(149);
    ASSERT_EQ(rect.getWidth(), 150);
    ASSERT_EQ(rect.getMinX(), 0);
    ASSERT_EQ(rect.getMaxX(), 149);
    rect.setMinX(90);
    ASSERT_EQ(rect.getWidth(), 60);
    ASSERT_EQ(rect.getMinX(), 90);
    ASSERT_EQ(rect.getMaxX(), 149);

    rect.setMaxY(149);
    ASSERT_EQ(rect.getHeight(), 150);
    ASSERT_EQ(rect.getMinY(), 0);
    ASSERT_EQ(rect.getMaxY(), 149);
    rect.setMinY(90);
    ASSERT_EQ(rect.getHeight(), 60);
    ASSERT_EQ(rect.getMinY(), 90);
    ASSERT_EQ(rect.getMaxY(), 149);

}