#include "stun_dag/temperature.h"
#include <catch2/catch.hpp>

TEST_CASE("Temperature type traits")
{
    using angonoka::stun_dag::Temperature;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Temperature>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Temperature>);
    STATIC_REQUIRE(std::is_move_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_move_assignable_v<Temperature>);
}
