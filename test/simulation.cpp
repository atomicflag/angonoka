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

    SECTION("params")
    {
        const auto config = load_text(text);
        const std::vector<stun::ScheduleItem> schedule{
            {0, 0},
            {1, 1}};
        stun::RandomUtils random{0};

        detail::Simulation sim{{.config{&config}, .random{&random}}};

        const auto params = sim.params();

        REQUIRE(params.random == &random);
        REQUIRE(params.config == &config);

        stun::RandomUtils random2{42};
        sim.params({.config{&config}, .random{&random2}});
        REQUIRE(sim(schedule) == 10043s);
        REQUIRE(sim.params().random == &random2);
    }

    SECTION("average")
    {
        const auto config = load_text(text);
        const std::vector<stun::ScheduleItem> schedule{
            {0, 0},
            {1, 1}};
        stun::RandomUtils random{0};

        detail::Simulation sim{{.config{&config}, .random{&random}}};

        float count = 1.F;
        float avg = static_cast<float>(sim(schedule).count());
        for (int i{0}; i < 100; ++i) {
            const auto v = static_cast<float>(sim(schedule).count());
            avg = avg * (count / (count + 1.F))
                + v * (1.F / (count + 1.F));
            ++count;
        }

        REQUIRE(avg == Approx(9546.38F));
    }

    SECTION("negative values")
    {
        // The simulation uses normal distribution
        // to pick values for performance and duration.
        // Due to the fact that the normal distribution
        // is unbounded, some values might fall below
        // or equal to 0, which violates the constraints
        // of our statistical model.
        //
        // This test checks if the simulation picks new
        // values in case that happens.

        // clang-format off
        constexpr auto text_neg = 
            "agents:\n"
            "  Bob:\n"
            "    performance:\n"
            "      min: 0.001\n"
            "      max: 2.0\n"
            "tasks:\n"
            "  - name: Task 1\n"
            "    duration:\n"
            "      min: 1s\n"
            "      max: 1d\n";
        // clang-format on

        const auto config = load_text(text_neg);
        const std::vector<stun::ScheduleItem> schedule{{0, 0}};
        stun::RandomUtils random{0};

        detail::Simulation sim{{.config{&config}, .random{&random}}};

        float min = 9999.F;

        for (int i{0}; i < 100; ++i) {
            const auto v = static_cast<float>(sim(schedule).count());
            min = std::min(min, v);
        }

        REQUIRE(min == Approx(1517.F));
    }
}

TEST_CASE("histogram")
{
    using namespace angonoka;

    SECTION("granularity")
    {
        using angonoka::detail::granularity;
        using namespace std::chrono_literals;
        using std::chrono::days;

        REQUIRE(granularity(1h) == 60.F);
        REQUIRE(granularity(4h) == 60.F);
        REQUIRE(granularity(5h) == 3600.F);
        REQUIRE(granularity(days{12}) == 3600.F);
        REQUIRE(granularity(days{13}) == 86400.F);
    }

    SECTION("basic histogram")
    {
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

        const auto config = load_text(text);
        const OptimizedSchedule schedule{.schedule{{0, 0}, {1, 1}}};
        histogram(config, schedule);
        // TODO: wip
    }
}
