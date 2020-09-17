#include "stun/task_duration_cache.h"
#include <catch2/catch.hpp>

// TODO: Add more tests

TEST_CASE("TaskDurations type traits")
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
