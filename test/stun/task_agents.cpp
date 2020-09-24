#include "stun/task_agents.h"
#include <catch2/catch.hpp>

TEST_CASE("TaskAgents type traits")
{
    using angonoka::stun::TaskAgents;
    static_assert(std::is_nothrow_destructible_v<TaskAgents>);
    static_assert(
        std::is_nothrow_default_constructible_v<TaskAgents>);
    static_assert(!std::is_copy_constructible_v<TaskAgents>);
    static_assert(!std::is_copy_assignable_v<TaskAgents>);
    static_assert(std::is_nothrow_move_constructible_v<TaskAgents>);
    static_assert(std::is_nothrow_move_assignable_v<TaskAgents>);
}
