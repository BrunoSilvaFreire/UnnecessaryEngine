
#include <gtest/gtest.h>
#include <unnecessary/misc/historic.h>

TEST(historics, boolean_historics_signals) {
    un::BooleanHistoric historic;
    historic.set(true);
    ASSERT_TRUE(historic.wasJustActivated());
    historic.set(true);
    ASSERT_FALSE(historic.wasJustActivated());
    historic.set(false);
    ASSERT_TRUE(historic.wasJustDeactivated());
    historic.set(false);
    ASSERT_FALSE(historic.wasJustDeactivated());
}