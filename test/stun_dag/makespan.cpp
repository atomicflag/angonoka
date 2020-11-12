#include "stun_dag/makespan.h"
#include "stun_dag/schedule_info.h"
#include <catch2/catch.hpp>

TEST_CASE("Makespan type traits")
{
    using angonoka::stun_dag::Makespan;
    // TODO: Fix static_assert -> STATIC_REQUIRE everywhere
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Makespan>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Makespan>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Makespan>);
}
