#undef UNIT_TEST
#include "stun/optimizer.h"
#include <catch2/catch.hpp>

TEST_CASE("Optimizer type traits")
{
    using angonoka::stun::Optimizer;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Optimizer>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Optimizer>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Optimizer>);
}

// TODO: Fix ODR
