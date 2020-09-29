#include "stun/random_utils.h"
#include <catch2/catch.hpp>

TEST_CASE("RandomUtils type traits")
{
    using angonoka::stun::RandomUtils;
    static_assert(std::is_nothrow_destructible_v<RandomUtils>);
    static_assert(!std::is_default_constructible_v<RandomUtils>);
    static_assert(std::is_copy_constructible_v<RandomUtils>);
    static_assert(std::is_copy_assignable_v<RandomUtils>);
    static_assert(std::is_nothrow_move_constructible_v<RandomUtils>);
    static_assert(std::is_nothrow_move_assignable_v<RandomUtils>);
}
