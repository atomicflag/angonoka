#include <catch2/catch.hpp>
#include <type_traits>

#include "src/py/random_utils.h"
#include "src/py/task_agents.h"

TEST_CASE("RandomUtils type traits")
{
    using angonoka::stun::RandomUtils;
    static_assert(std::is_nothrow_destructible_v<RandomUtils>);
    static_assert(std::is_default_constructible_v<RandomUtils>);
    static_assert(std::is_nothrow_copy_constructible_v<RandomUtils>);
    static_assert(std::is_nothrow_copy_assignable_v<RandomUtils>);
    static_assert(std::is_nothrow_move_constructible_v<RandomUtils>);
    static_assert(std::is_nothrow_move_assignable_v<RandomUtils>);
}

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

TEST_CASE("BetaDriver type traits")
{
    using angonoka::stun::BetaDriver;
    static_assert(std::is_nothrow_destructible_v<BetaDriver>);
    static_assert(!std::is_default_constructible_v<BetaDriver>);
    static_assert(std::is_nothrow_copy_constructible_v<BetaDriver>);
    static_assert(std::is_nothrow_copy_assignable_v<BetaDriver>);
    static_assert(std::is_nothrow_move_constructible_v<BetaDriver>);
    static_assert(std::is_nothrow_move_assignable_v<BetaDriver>);
}

TEST_CASE("StochasticTunneling type traits")
{
    using angonoka::stun::StochasticTunneling;
    static_assert(
        std::is_nothrow_destructible_v<StochasticTunneling>);
    static_assert(
        !std::is_default_constructible_v<StochasticTunneling>);
    static_assert(!std::is_copy_constructible_v<StochasticTunneling>);
    static_assert(!std::is_copy_assignable_v<StochasticTunneling>);
    static_assert(
        std::is_nothrow_move_constructible_v<StochasticTunneling>);
    static_assert(
        std::is_nothrow_move_assignable_v<StochasticTunneling>);
}
