#include "stun_dag/common.h"
#include <catch2/catch.hpp>

TEST_CASE("StateItem printing")
{
    using angonoka::stun_dag::StateItem;

    const StateItem item{42, 123};

    REQUIRE(fmt::format("{}", item) == "(42, 123)");
}

TEST_CASE("StateItem type traits")
{
    using angonoka::stun_dag::StateItem;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<StateItem>);
    STATIC_REQUIRE(std::is_default_constructible_v<StateItem>);
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<StateItem>);
    STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<StateItem>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<StateItem>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<StateItem>);
    STATIC_REQUIRE(std::is_trivially_copyable_v<StateItem>);
}
