#include <catch2/catch.hpp>
#include <type_traits>

#include "src/py/random_utils.h"
#include "src/py/task_agents.h"

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
