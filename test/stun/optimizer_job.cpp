#include "stun/optimizer_job.h"
#include "config/load.h"
#include <catch2/catch.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

namespace {
using angonoka::stun::OptimizerJob;
OptimizerJob make(auto& params, auto& random)
{
    return OptimizerJob{
        {.params{&params},
         .random{&random},
         .batch_size{5},
         .beta_scale{1e-4F},
         .stun_window{10000},
         .gamma{.5F},
         .restart_period{1 << 20}}};
}
} // namespace

#pragma clang diagnostic pop

TEST_CASE("OptimizerJob")
{
    SECTION("OptimizerJob type traits")
    {
        using angonoka::stun::OptimizerJob;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<OptimizerJob>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<OptimizerJob>);
        STATIC_REQUIRE(std::is_copy_constructible_v<OptimizerJob>);
        STATIC_REQUIRE(std::is_copy_assignable_v<OptimizerJob>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<OptimizerJob>);
        STATIC_REQUIRE(
            std::is_nothrow_move_assignable_v<OptimizerJob>);
    }

    SECTION("basic OptimizerJob operations")
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
        RandomUtils random;
        OptimizerJob optimizer = make(params, random);

        REQUIRE(optimizer.normalized_makespan() == 2.F);
        REQUIRE(optimizer.schedule()[1].agent_id == 0);

        optimizer.update();

        while (optimizer.normalized_makespan() != 1.F)
            optimizer.update();

        // Might be non-deterministic
        REQUIRE(optimizer.normalized_makespan() == 1.F);
        // Each task has a different agent
        REQUIRE(
            optimizer.schedule()[1].agent_id
            != optimizer.schedule()[0].agent_id);

        SECTION("reset")
        {
            optimizer.reset();

            REQUIRE(optimizer.normalized_makespan() == 2.F);
        };

        SECTION("rebind params")
        {
            REQUIRE(optimizer.params().params == &params);
            REQUIRE(optimizer.params().random == &random);
            const auto params2 = params;
            optimizer.params({.params{&params2}, .random{&random}});
            REQUIRE(optimizer.params().params == &params2);

            while (optimizer.normalized_makespan() != 1.F)
                optimizer.update();
        };
    };

    SECTION("OptimizerJob special memeber functions")
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
        RandomUtils random;

        OptimizerJob job = make(params, random);

        SECTION("copy ctor")
        {
            OptimizerJob other{job};

            REQUIRE(other.normalized_makespan() == 2.F);

            job.update();

            REQUIRE(other.normalized_makespan() == 2.F);
        };

        SECTION("copy assignment")
        {
            OptimizerJob other = make(params, random);
            other = job;

            REQUIRE(other.normalized_makespan() == 2.F);

            job.update();

            REQUIRE(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            REQUIRE(other.normalized_makespan() == 1.F);

            {
                OptimizerJob job2 = make(params, random);
                other = job2;
            }

            REQUIRE(other.normalized_makespan() == 2.F);
        };

        SECTION("move ctor")
        {
            OptimizerJob other{std::move(job)};

            REQUIRE(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            REQUIRE(other.normalized_makespan() == 1.F);
        };

        SECTION("move assignment")
        {
            OptimizerJob other = make(params, random);
            other = std::move(job);

            REQUIRE(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            REQUIRE(other.normalized_makespan() == 1.F);
        };

        SECTION("destructive move assignment")
        {
            REQUIRE(job.normalized_makespan() == 2.F);

            {
                OptimizerJob other = make(params, random);
                job = std::move(other);
            }

            REQUIRE(job.normalized_makespan() == 2.F);

            while (job.normalized_makespan() != 1.F) job.update();

            REQUIRE(job.normalized_makespan() == 1.F);
        };

        SECTION("self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            job = job;
#pragma clang diagnostic pop

            REQUIRE(job.normalized_makespan() == 2.F);
        };

        SECTION("self move")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            job = std::move(job);
#pragma clang diagnostic pop

            REQUIRE(job.normalized_makespan() == 2.F);
        };
    };
};
