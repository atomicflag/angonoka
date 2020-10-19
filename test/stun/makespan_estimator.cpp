#include "stun/makespan_estimator.h"
#include "stun/task_duration_cache.h"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("MakespanEstimator type traits")
{
    using angonoka::stun::MakespanEstimator;
    static_assert(std::is_nothrow_destructible_v<MakespanEstimator>);
    static_assert(
        !std::is_default_constructible_v<MakespanEstimator>);
    static_assert(std::is_copy_constructible_v<MakespanEstimator>);
    static_assert(std::is_copy_assignable_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_constructible_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_assignable_v<MakespanEstimator>);
}

TEST_CASE("MakespanEstimator estimation")
{
    using namespace angonoka::stun;

    const std::vector<float> data{1.F, 2.F, 3.F};
    // TODO: stubs
    const TaskDurationCache cache{data, data};

    MakespanEstimator estimator{3, &cache};

    REQUIRE(estimator(std::vector<int16>{1, 2, 1}) == 2.0F);
    REQUIRE(estimator(std::vector<int16>{0, 1, 2}) == 1.0F);
    REQUIRE(estimator(std::vector<int16>{2, 1, 0}) == 3.0F);
}
