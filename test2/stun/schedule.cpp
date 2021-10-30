#include "stun/schedule.h"
#include <catch2/catch.hpp>

TEST_CASE("ScheduleItem")
{
    SECTION("ScheduleItem printing")
    {
        using angonoka::stun::ScheduleItem;

        const ScheduleItem item{42, 123};

        REQUIRE(fmt::format("{}", item) == "(42, 123)");
    }

    SECTION("ScheduleItem type traits")
    {
        using angonoka::stun::ScheduleItem;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<ScheduleItem>);
        STATIC_REQUIRE(std::is_default_constructible_v<ScheduleItem>);
        STATIC_REQUIRE(
            std::is_nothrow_copy_constructible_v<ScheduleItem>);
        STATIC_REQUIRE(
            std::is_nothrow_copy_assignable_v<ScheduleItem>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<ScheduleItem>);
        STATIC_REQUIRE(
            std::is_nothrow_move_assignable_v<ScheduleItem>);
        STATIC_REQUIRE(std::is_trivially_copyable_v<ScheduleItem>);
    }
}
