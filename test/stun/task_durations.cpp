#include "stun/task_durations.h"
#include <catch2/catch.hpp>

TEST_CASE("TaskDurations type traits")
{
    using angonoka::stun::TaskDurations;
    static_assert(std::is_nothrow_destructible_v<TaskDurations>);
    static_assert(
        std::is_nothrow_default_constructible_v<TaskDurations>);
    static_assert(!std::is_copy_constructible_v<TaskDurations>);
    static_assert(!std::is_copy_assignable_v<TaskDurations>);
    static_assert(
        std::is_nothrow_move_constructible_v<TaskDurations>);
    static_assert(std::is_nothrow_move_assignable_v<TaskDurations>);
}
