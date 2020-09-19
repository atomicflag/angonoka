#include "stun/task_duration_cache.h"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("TaskDurationCache type traits")
{
    using angonoka::stun::TaskDurationCache;
    static_assert(std::is_nothrow_destructible_v<TaskDurationCache>);
    static_assert(
        std::is_nothrow_default_constructible_v<TaskDurationCache>);
    static_assert(!std::is_copy_constructible_v<TaskDurationCache>);
    static_assert(!std::is_copy_assignable_v<TaskDurationCache>);
    static_assert(
        std::is_nothrow_move_constructible_v<TaskDurationCache>);
    static_assert(
        std::is_nothrow_move_assignable_v<TaskDurationCache>);
}

TEST_CASE("TaskDurationCache values")
{
    using angonoka::stun::AgentIndex;
    using angonoka::stun::TaskDurationCache;
    using angonoka::stun::TaskIndex;

    const std::vector<float> data{1.f, 2.f, 3.f};

    const TaskDurationCache cache{data, data};

    REQUIRE(cache.get(AgentIndex{0}, TaskIndex{0}) == 1.f);
    REQUIRE(cache.get(AgentIndex{1}, TaskIndex{1}) == 1.f);
    REQUIRE(cache.get(AgentIndex{2}, TaskIndex{2}) == 1.f);
    REQUIRE(cache.get(AgentIndex{0}, TaskIndex{2}) == 3.f);
}
