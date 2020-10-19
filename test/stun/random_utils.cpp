#include "stun/random_utils.h"
#include "stun/task_agents.h"
#include <catch2/catch.hpp>
#include <functional>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/zip_with.hpp>
#include <vector>

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

TEST_CASE("RandomUtils methods")
{
    using namespace angonoka::stun;
    using ranges::to;
    using ranges::views::chunk;

    const std::vector<int16> data{0, 1, 2, 3, 4, 5, 6, 7, 8};
    const auto spans
        = data | chunk(3) | to<std::vector<span<const int16>>>();

    const TaskAgents task_agents{spans};

    // TODO: stubs
    RandomUtils utils{&task_agents};

    SECTION("Uniform value")
    {
        const auto value = utils.get_uniform();
        REQUIRE((value >= 0.f && value <= 1.f));
    }

    SECTION("Get neighbor")
    {
        const auto values = {3, 6, 1};
        auto state = values | to<std::vector<int16>>();

        utils.get_neighbor_inplace(state);

        using ranges::accumulate;
        using ranges::views::zip_with;

        const auto diff = accumulate(
            zip_with(std::not_equal_to<>{}, state, values),
            0);

        REQUIRE(diff == 1);
    }
}
