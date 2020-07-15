#include "schedule.h"
#include <catch2/catch.hpp>
#include <random>

namespace {
angonoka::System make_system()
{
    using namespace angonoka;
    using namespace std::literals::chrono_literals;
    return {
        .groups{"A", "B"},
        .agents{
            {.name{"Agent 1"},
             .group_ids{GroupId{0}},
             .performance{0.5f, 1.0f}},
            {.name{"Agent 2"},
             .group_ids{GroupId{1}},
             .performance{0.6f, 0.9f}},
            {.name{"Agent 3"},
             .group_ids{},
             .performance{0.9f, 1.1f}}},
        .tasks{
            {.name{"Task 1"},
             .group_id{GroupId{0}},
             .duration{15s, 30s}},
            {.name{"Task 2"},
             .group_id{GroupId{1}},
             .duration{35s, 50s}},
            {.name{"Task 3"}, .group_id{}, .duration{10s, 40s}},
            {.name{"Task 4"},
             .group_id{GroupId{1}},
             .duration{40s, 90s}}}};
}
} // namespace

TEST_CASE("Constraints type traits")
{
    using angonoka::detail::Constraints;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Constraints>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Constraints>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Constraints>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Constraints>);
    STATIC_REQUIRE(std::is_move_constructible_v<Constraints>);
    STATIC_REQUIRE(std::is_move_assignable_v<Constraints>);
}

TEST_CASE("Schedule")
{
    using namespace angonoka;
    using IndData = std::vector<std::int_fast8_t>;

    SECTION("Constraints")
    {
        const auto sys = make_system();
        const detail::Constraints cons{sys};

        REQUIRE_FALSE(cons.durations.empty());
        REQUIRE(cons.durations.size() == 4);
        REQUIRE(
            cons.durations
            == detail::ExpectedDurations{22, 42, 25, 65});

        REQUIRE_FALSE(cons.performance.empty());
        REQUIRE(cons.performance.size() == 3);
        REQUIRE(cons.performance[0] == Approx(0.75f));
        REQUIRE(cons.performance[1] == Approx(0.75f));
        REQUIRE(cons.performance[2] == Approx(1.0f));

        REQUIRE(cons.can_work_on(0, 0));
        REQUIRE_FALSE(cons.can_work_on(0, 1));
        REQUIRE(cons.can_work_on(0, 2));
        REQUIRE(cons.can_work_on(2, 2));
        REQUIRE_FALSE(cons.can_work_on(1, 0));
        REQUIRE(cons.can_work_on(1, 2));
    }

    SECTION("Makespan")
    {
        const auto sys = make_system();
        const detail::Constraints cons{sys};
        std::vector<std::int_fast32_t> buf(3);
        {
            const IndData ind{0, 1, 2, 1};
            REQUIRE(makespan(ind, cons, buf) == 80);
        }
        {
            const IndData ind{2, 2, 2, 2};
            REQUIRE(makespan(ind, cons, buf) == 154);
        }
    }

    SECTION("Crossover")
    {
        detail::RandomEngine gen{0};
        IndData child(4), p1(4, 0), p2(4, 1), p3(4, 2);

        detail::crossover({p1, p2, p3}, child, gen);

        REQUIRE(child == IndData{2, 1, 2, 0});
    }
}
