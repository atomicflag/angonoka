#include "stun/optimizer.h"
#include "config/load.h"
#include <catch2/catch.hpp>

TEST_CASE("Optimizer")
{
    SECTION("Optimizer type traits")
    {
        using angonoka::stun::Optimizer;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<Optimizer>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<Optimizer>);
        STATIC_REQUIRE(std::is_copy_constructible_v<Optimizer>);
        STATIC_REQUIRE(std::is_copy_assignable_v<Optimizer>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<Optimizer>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Optimizer>);
    }

    SECTION("basic Optimizer operations")
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
        Optimizer optimizer{
            {.params{&params}, .batch_size{5}, .max_idle_iters{10}}};

        REQUIRE(optimizer.normalized_makespan() == 2.F);
        REQUIRE(optimizer.estimated_progress() == 0.F);
        REQUIRE(optimizer.schedule()[1].agent_id == 0);

        optimizer.update();

        REQUIRE(optimizer.estimated_progress() == 0.F);

        while (!optimizer.has_converged()) optimizer.update();

        // Might be non-deterministic
        REQUIRE(optimizer.normalized_makespan() == 1.F);
        REQUIRE(optimizer.estimated_progress() == 1.F);
        // Each task has a different agent
        REQUIRE(
            optimizer.schedule()[1].agent_id
            != optimizer.schedule()[0].agent_id);

        SECTION("reset")
        {
            optimizer.reset();

            REQUIRE(optimizer.normalized_makespan() == 2.F);
            REQUIRE(optimizer.estimated_progress() == 0.F);
        }

        SECTION("rebind params")
        {
            REQUIRE(&optimizer.params() == &params);
            const auto params2 = params;
            optimizer.params(params2);
            REQUIRE(&optimizer.params() == &params2);

            while (!optimizer.has_converged()) optimizer.update();
        }
    }

    SECTION("Optimizer special memeber functions")
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

        Optimizer optimizer{
            {.params{&params}, .batch_size{5}, .max_idle_iters{10}}};

        SECTION("copy ctor")
        {
            Optimizer other{optimizer};

            REQUIRE(other.normalized_makespan() == 2.F);

            while (!optimizer.has_converged()) optimizer.update();

            REQUIRE(other.normalized_makespan() == 2.F);
        }

        SECTION("copy assignment")
        {
            Optimizer other{
                {.params{&params},
                 .batch_size{5},
                 .max_idle_iters{10}}};
            other = optimizer;

            REQUIRE(other.normalized_makespan() == 2.F);

            while (!optimizer.has_converged()) optimizer.update();

            REQUIRE(other.normalized_makespan() == 2.F);

            while (!other.has_converged()) other.update();
            REQUIRE(other.normalized_makespan() == 1.F);

            {
                Optimizer optimizer2{
                    {.params{&params},
                     .batch_size{5},
                     .max_idle_iters{10}}};
                other = optimizer2;
            }

            REQUIRE(other.normalized_makespan() == 2.F);
        }

        SECTION("move ctor")
        {
            Optimizer other{std::move(optimizer)};

            REQUIRE(other.normalized_makespan() == 2.F);

            while (!other.has_converged()) other.update();

            REQUIRE(other.normalized_makespan() == 1.F);
        }

        SECTION("move assignment")
        {
            Optimizer other{
                {.params{&params},
                 .batch_size{5},
                 .max_idle_iters{10}}};
            other = std::move(optimizer);

            REQUIRE(other.normalized_makespan() == 2.F);

            while (!other.has_converged()) other.update();

            REQUIRE(other.normalized_makespan() == 1.F);
        }

        SECTION("destructive move assignment")
        {
            REQUIRE(optimizer.normalized_makespan() == 2.F);

            {
                Optimizer other{
                    {.params{&params},
                     .batch_size{5},
                     .max_idle_iters{10}}};
                optimizer = std::move(other);
            }

            REQUIRE(optimizer.normalized_makespan() == 2.F);

            while (!optimizer.has_converged()) optimizer.update();

            REQUIRE(optimizer.normalized_makespan() == 1.F);
        }

        SECTION("self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            optimizer = optimizer;
#pragma clang diagnostic pop

            REQUIRE(optimizer.normalized_makespan() == 2.F);
        }

        // self move is not supported
    }
}
