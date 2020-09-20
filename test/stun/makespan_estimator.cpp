#include "stun/makespan_estimator.h"
#include <catch2/catch.hpp>

TEST_CASE("MakespanEstimator type traits")
{
    using angonoka::stun::MakespanEstimator;
    static_assert(std::is_nothrow_destructible_v<MakespanEstimator>);
    static_assert(
        !std::is_default_constructible_v<MakespanEstimator>);
    static_assert(!std::is_copy_constructible_v<MakespanEstimator>);
    static_assert(!std::is_copy_assignable_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_constructible_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_assignable_v<MakespanEstimator>);
}
