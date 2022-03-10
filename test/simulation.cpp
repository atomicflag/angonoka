#include "simulation.h"
#include "config/load.h"
#include "predict.h"
#include <catch2/catch.hpp>

TEST_CASE("Simulation")
{
    using namespace angonoka;
    using namespace std::literals::chrono_literals;

    // clang-format off
    constexpr auto text = 
        "agents:\n"
        "  Bob:\n"
        "    performance:\n"
        "      min: 0.5\n"
        "      max: 1.5\n"
        "  Jack:\n"
        "    performance:\n"
        "      min: 0.5\n"
        "      max: 1.5\n"
        "tasks:\n"
        "  - name: Task 1\n"
        "    duration:\n"
        "      min: 1h\n"
        "      max: 3h\n"
        "  - name: Task 2\n"
        "    duration:\n"
        "      min: 1h\n"
        "      max: 3h\n";
    // clang-format on

    SECTION("Simulation type traits")
    {
        using angonoka::detail::Simulation;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<Simulation>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<Simulation>);
        STATIC_REQUIRE(std::is_copy_constructible_v<Simulation>);
        STATIC_REQUIRE(std::is_copy_assignable_v<Simulation>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<Simulation>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Simulation>);
    }

    SECTION("running simulation")
    {
        const auto config = load_text(text);
        const std::vector<stun::ScheduleItem> schedule{
            {0, 0},
            {1, 1}};
        stun::RandomUtils random{0};

        detail::Simulation sim{{.config{&config}, .random{&random}}};
        const auto duration = sim(schedule);

        // Expected duration is 2h (7200s)
        REQUIRE(duration == 7397s);
    }

    SECTION("Simulation special memeber functions")
    {
        using detail::Simulation;

        const auto config = load_text(text);
        const std::vector<stun::ScheduleItem> schedule{
            {0, 0},
            {1, 1}};
        stun::RandomUtils random{0};
        Simulation sim{{.config{&config}, .random{&random}}};

        SECTION("copy ctor")
        {
            Simulation other{sim};

            REQUIRE(other(schedule) == 7397s);
            REQUIRE(sim(schedule) == 6474s);
        }

        SECTION("copy assignment")
        {
            const auto config2 = load_text(text);
            stun::RandomUtils random2{42};
            Simulation other{{.config{&config2}, .random{&random2}}};
            other = sim;

            REQUIRE(other(schedule) == 7397s);
            REQUIRE(sim(schedule) == 6474s);
        }

        SECTION("move ctor")
        {
            Simulation other{std::move(sim)};

            REQUIRE(other(schedule) == 7397s);
        }

        SECTION("move assignment")
        {
            const auto config2 = load_text(text);
            stun::RandomUtils random2{42};
            Simulation other{{.config{&config2}, .random{&random2}}};
            other = std::move(sim);

            REQUIRE(other(schedule) == 7397s);
        }

        // Don't want to bother with self-move or self-copy
    }
}
