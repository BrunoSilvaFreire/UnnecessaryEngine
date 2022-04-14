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
    ASSERT_TRUE(rect.getMaxY() == maxLimit) << rect << " @ " << rect.getMaxY() << ", " << excluded << ", " << maxLimit;
    ASSERT_TRUE(rect.getMinY() == cutoff) << rect << " @ " << rect.getMinY() << ", " << cutoff;
    ASSERT_TRUE(rect.getMinX() == cutoff) << rect << " @ " << rect.getMinX() << ", " << cutoff;
    ASSERT_TRUE(rect.getMaxX() == maxLimit) << rect << " @ " << rect.getMaxX() << ", " << excluded << ", " << maxLimit;
}

TEST(rects, exclude) {
    const u32 cutoff = 10;
    const u32 size = 100;
    un::uRect rect(0, 0, size, size);
    u32 maxLimit = size - 1;
    un::uRect inset(cutoff, 0, size - cutoff, size);
    GTEST_LOG_(INFO) << "rect: " << rect;
    GTEST_LOG_(INFO) << "inset: " << inset;
    rect.exclude(inset);
    ASSERT_TRUE(rect.getMaxY() == maxLimit) << rect << " @ " << rect.getMaxY() << ", " << maxLimit;
    ASSERT_TRUE(rect.getMinY() == 0) << rect << " @ " << rect.getMinY();
    ASSERT_TRUE(rect.getMinX() == cutoff) << rect << " @ " << rect.getMinX() << ", " << cutoff;
    ASSERT_TRUE(rect.getMaxX() == maxLimit) << rect << " @ " << rect.getMaxX() << ", ";
}

TEST(rects, set_max_min) {
    u32 size = 100;
    un::uRect rect(0, 0, size, size);
    ASSERT_TRUE(rect.getMaxY() == size - 1) << rect;
    ASSERT_TRUE(rect.getMinY() == 0) << rect;
    ASSERT_TRUE(rect.getMaxX() == size - 1) << rect;
    ASSERT_TRUE(rect.getMinX() == 0) << rect;
    rect.setMaxX(149);
    ASSERT_TRUE(rect.getWidth() == 150);
    ASSERT_TRUE(rect.getMinX() == 0);
    ASSERT_TRUE(rect.getMaxX() == 149);
    rect.setMinX(90);
    ASSERT_TRUE(rect.getWidth() == 60);
    ASSERT_TRUE(rect.getMinX() == 90);
    ASSERT_TRUE(rect.getMaxX() == 149);

    rect.setMaxY(149);
    ASSERT_TRUE(rect.getHeight() == 150);
    ASSERT_TRUE(rect.getMinY() == 0);
    ASSERT_TRUE(rect.getMaxY() == 149);
    rect.setMinY(90);
    ASSERT_TRUE(rect.getHeight() == 60);
    ASSERT_TRUE(rect.getMinY() == 90);
    ASSERT_TRUE(rect.getMaxY() == 149);

}