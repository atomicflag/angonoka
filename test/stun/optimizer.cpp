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

    while (!optimizer.has_converged()) optimizer.update();

    // Might be non-deterministic
    REQUIRE(optimizer.energy() == 1.F);
}
