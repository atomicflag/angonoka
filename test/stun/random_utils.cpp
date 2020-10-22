#include "stun/random_utils.h"
#include "stun/task_agents.h"
#include "utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <functional>
#include <range/v3/algorithm/equal.hpp>
#include <vector>

namespace {
using namespace angonoka::stun;
using angonoka::stun::index;
struct TaskAgentsMock final : TaskAgentsStub {
    span<const int16> operator[](index i) const noexcept override
    {
        return get(i);
    }

    MAKE_CONST_MOCK1(get, span<const int16>(index), noexcept);
};
} // namespace

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
    using angonoka::stun::index;
    using angonoka::utils::make_array;
    using ranges::equal;

    const TaskAgentsMock task_agents;

    RandomUtils utils{&task_agents, 1};

    SECTION("Uniform value")
    {
        const auto value = utils.get_uniform();
        REQUIRE((value >= 0.f && value <= 1.f));
    }

    SECTION("Get neighbor")
    {
        const auto data = make_array<int16>(2);
        REQUIRE_CALL(task_agents, get(index{0}))
            .RETURN(span<const int16>{data});

        std::vector<int16> state{3, 6, 1};

        utils.get_neighbor_inplace(state);

        REQUIRE(equal(state, make_array<int16>(2, 6, 1)));
    }
}
