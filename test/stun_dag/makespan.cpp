#include "stun_dag/schedule_info.h"
#include "stun_dag/utils.h"
#include <catch2/catch.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>

namespace {
using namespace angonoka::stun_dag;
ScheduleInfo make_test_schedule_info()
{
    using ranges::to;
    using ranges::actions::push_back;
    using ranges::views::chunk;

    ScheduleInfo info;
    info.agent_performance = {1.F, 2.F, 3.F};
    info.task_duration = {1.F, 2.F, 3.F};
    info.available_agents_data = {0, 1, 0, 1, 0, 1};
    info.available_agents = info.available_agents_data | chunk(2)
        | to<decltype(info.available_agents)>();

    info.dependencies_data = {0, 1};
    info.dependencies.emplace_back();
    push_back(info.dependencies, info.dependencies_data | chunk(1));
    return info;
}
} // namespace

TEST_CASE("Makespan type traits")
{
    using angonoka::stun_dag::Makespan;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Makespan>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Makespan>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Makespan>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Makespan>);
}

TEST_CASE("Makespan special member functions")
{
    using namespace angonoka::stun_dag;

    const auto info = make_test_schedule_info();
    const std::vector<StateItem> state{{0, 0}, {1, 1}, {2, 2}};
    const std::vector<StateItem> state2{{0, 0}, {1, 1}};

    Makespan makespan{&info, TasksCount{3}, AgentsCount{3}};
    REQUIRE(makespan(state) == Approx(3.F));
    Makespan makespan2{&info, TasksCount{2}, AgentsCount{2}};
    REQUIRE(makespan2(state2) == Approx(2.F));

    SECTION("Copy assignment")
    {
        makespan2 = makespan;
        REQUIRE(makespan2(state) == Approx(3.F));
    }
    SECTION("Move assignment")
    {
        makespan2 = std::move(makespan);
        REQUIRE(makespan2(state) == Approx(3.F));
    }
    SECTION("Copy ctor")
    {
        Makespan makespan3{makespan2};
        REQUIRE(makespan3(state2) == Approx(2.F));
    }
    SECTION("Move ctor")
    {
        Makespan makespan4{std::move(makespan2)};
        REQUIRE(makespan4(state2) == Approx(2.F));
    }
}

TEST_CASE("Makespan estimation")
{
    const auto info = make_test_schedule_info();
    Makespan makespan{&info, TasksCount{3}, AgentsCount{3}};
    const std::vector<StateItem> state{{0, 0}, {1, 1}, {2, 2}};
    REQUIRE(makespan(state) == Approx(3.F));
}
