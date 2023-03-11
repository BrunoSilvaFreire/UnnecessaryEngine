#include <gtest/gtest.h>
#include <unnecessary/misc/historic.h>

TEST(historics, boolean_historics_signals) {
    un::boolean_historic historic;
    historic.set(true);
    ASSERT_TRUE(historic.was_just_activated());
    historic.set(true);
    ASSERT_FALSE(historic.was_just_activated());
    historic.set(false);
    ASSERT_TRUE(historic.was_just_deactivated());
    historic.set(false);
    ASSERT_FALSE(historic.was_just_deactivated());
}
