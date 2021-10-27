#include "stun/schedule_params.h"
#include "stun/utils.h"
#include <catch2/catch.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>

namespace {
using namespace angonoka::stun;
ScheduleParams make_test_schedule_params()
{
    using ranges::to;
    using ranges::actions::push_back;
    using ranges::views::chunk;

    ScheduleParams params;
    params.agent_performance = {1.F, 2.F, 3.F};
    params.task_duration = {1.F, 2.F, 3.F};
    std::vector<int16> available_agents_data = {0, 1, 0, 1, 0, 1};
    auto available_agents = available_agents_data | chunk(2)
        | to<std::vector<span<int16>>>();
    params.available_agents
        = {std::move(available_agents_data),
           std::move(available_agents)};

    std::vector<int16> dependencies_data = {0, 1};
    std::vector<span<int16>> dependencies{{}};
    push_back(dependencies, dependencies_data | chunk(1));
    params.dependencies
        = {std::move(dependencies_data), std::move(dependencies)};
    return params;
}
} // namespace

TEST_CASE("Makespan")
{
    SECTION("Makespan type traits")
    {
        using angonoka::stun::Makespan;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<Makespan>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<Makespan>);
        STATIC_REQUIRE(std::is_copy_constructible_v<Makespan>);
        STATIC_REQUIRE(std::is_copy_assignable_v<Makespan>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<Makespan>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Makespan>);
    }

    SECTION("Makespan special member functions")
    {
        using namespace angonoka::stun;

        const auto params = make_test_schedule_params();
        const std::vector<ScheduleItem> schedule{
            {0, 0},
            {1, 1},
            {2, 2}};

        auto params2 = make_test_schedule_params();
        params2.agent_performance.resize(2);
        params2.task_duration.resize(2);

        Makespan makespan{params};

        REQUIRE(makespan(schedule) == Approx(3.));

        {
            const std::vector<ScheduleItem> schedule2{{0, 0}, {1, 1}};
            Makespan makespan2{params2};
            REQUIRE(makespan2(schedule2) == Approx(2.));
        }

        SECTION("copy assignment")
        {
            Makespan makespan2{params2};
            makespan2 = makespan;

            REQUIRE(makespan(schedule) == Approx(3.));
            REQUIRE(makespan2(schedule) == Approx(3.));
        }

        SECTION("move assignment")
        {
            Makespan makespan2{params2};
            makespan2 = std::move(makespan);

            REQUIRE(makespan2(schedule) == Approx(3.));
        }

        SECTION("copy ctor")
        {
            Makespan makespan2{makespan};

            REQUIRE(makespan2(schedule) == Approx(3.));
        }

        SECTION("move ctor")
        {
            Makespan makespan2{std::move(makespan)};

            REQUIRE(makespan2(schedule) == Approx(3.));
        }

        SECTION("self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            makespan = makespan;
#pragma clang diagnostic pop

            REQUIRE(makespan(schedule) == Approx(3.));
        }

        SECTION("self move")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            makespan = std::move(makespan);
#pragma clang diagnostic pop

            REQUIRE(makespan(schedule) == Approx(3.));
        }
    }

    SECTION("Makespan estimation")
    {
        const auto params = make_test_schedule_params();
        Makespan makespan{params};
        const std::vector<ScheduleItem> schedule{
            {0, 0},
            {1, 1},
            {2, 2}};

        REQUIRE(makespan(schedule) == Approx(3.));
    }

    SECTION("Makespan params")
    {
        const auto params = make_test_schedule_params();
        Makespan makespan{params};

        REQUIRE(&makespan.params() == &params);

        const auto params2 = make_test_schedule_params();
        makespan.params(params2);

        REQUIRE(&makespan.params() == &params2);

        const std::vector<ScheduleItem> schedule{
            {0, 0},
            {1, 1},
            {2, 2}};

        REQUIRE(makespan(schedule) == Approx(3.));
    }
}
