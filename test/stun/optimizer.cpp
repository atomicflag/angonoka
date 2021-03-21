#undef UNIT_TEST
#include "stun/optimizer.h"
#include "config/load.h"
#include <catch2/catch.hpp>

TEST_CASE("Optimizer type traits")
{
    using angonoka::stun::Optimizer;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Optimizer>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Optimizer>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Optimizer>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Optimizer>);
}

TEST_CASE("Basic Optimizer operations")
{
    using namespace angonoka::stun;

    // clang-format off
    constexpr auto text = 
        "agents:\n"
        "  Bob:\n"
        "  Jack:\n"
        "tasks:\n"
        "  - name: Task 1\n"
        "    duration: 1h\n"
        "  - name: Task 2\n"
        "    duration: 1h";
    // clang-format on
    const auto config = angonoka::load_text(text);

    const auto params = to_schedule_params(config);
    const auto schedule_params = to_schedule_params(config);
    Optimizer optimizer{params, BatchSize{5}, MaxIdleIters{10}};

    REQUIRE(optimizer.energy() == 2.F);
    REQUIRE(optimizer.estimated_progress() == 0.F);
    REQUIRE(optimizer.state()[1].agent_id == 0);

    optimizer.update();

    REQUIRE(optimizer.estimated_progress() > 0.F);

    while (!optimizer.has_converged()) optimizer.update();

    // Might be non-deterministic
    REQUIRE(optimizer.energy() == 1.F);
    REQUIRE(optimizer.estimated_progress() == 1.F);
    // Each task has a different agent
    REQUIRE(
        optimizer.state()[1].agent_id
        != optimizer.state()[0].agent_id);

    optimizer.reset();

    REQUIRE(optimizer.energy() == 2.F);
    REQUIRE(optimizer.estimated_progress() == 0.F);
}

TEST_CASE("Optimizer special memeber functions")
{
    using namespace angonoka::stun;
    //
    // clang-format off
    constexpr auto text = 
        "agents:\n"
        "  Bob:\n"
        "  Jack:\n"
        "tasks:\n"
        "  - name: Task 1\n"
        "    duration: 1h\n"
        "  - name: Task 2\n"
        "    duration: 1h";
    // clang-format on
    const auto config = angonoka::load_text(text);

    const auto params = to_schedule_params(config);
    const auto schedule_params = to_schedule_params(config);
    Optimizer optimizer{params, BatchSize{5}, MaxIdleIters{10}};

    SECTION("Copy ctor")
    {
        Optimizer other{optimizer};

        REQUIRE(other.energy() == 2.F);

        while (!optimizer.has_converged()) optimizer.update();

        REQUIRE(other.energy() == 2.F);
    }

    SECTION("Copy assignment")
    {
        Optimizer other{params, BatchSize{5}, MaxIdleIters{10}};
        other = optimizer;

        REQUIRE(other.energy() == 2.F);

        while (!optimizer.has_converged()) optimizer.update();

        REQUIRE(other.energy() == 2.F);
    }

    SECTION("Move ctor")
    {
        Optimizer other{std::move(optimizer)};

        REQUIRE(other.energy() == 2.F);

        while (!other.has_converged()) other.update();

        REQUIRE(other.energy() == 1.F);
    }

    SECTION("Move assignment")
    {
        Optimizer other{params, BatchSize{5}, MaxIdleIters{10}};
        other = std::move(optimizer);

        REQUIRE(other.energy() == 2.F);

        while (!other.has_converged()) other.update();

        REQUIRE(other.energy() == 1.F);
    }

    SECTION("Self copy")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
        optimizer = optimizer;
#pragma clang diagnostic pop

        REQUIRE(optimizer.energy() == 2.F);
    }

    SECTION("Self move")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
        optimizer = std::move(optimizer);
#pragma clang diagnostic pop

        REQUIRE(optimizer.energy() == 2.F);
    }
}
