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
        "    performance:\n"
        "      min: 0.5\n"
        "      max: 1.5\n"
        "  Jack:\n"
        "    groups:\n"
        "      - A\n"
        "tasks:\n"
        "  - name: Task 1\n"
        "    duration: 1h\n"
        "    id: 1\n"
        "    group: A\n"
        "  - name: Task 2\n"
        "    duration: 1h\n"
        "    depends_on: 1";
    // clang-format on
    const auto config = angonoka::load_text(text);

    const auto params = to_schedule_params(config);
    const auto schedule_params = to_schedule_params(config);
    Optimizer optimizer{params, BatchSize{1}, MaxIdleIters{1}};

    // TODO: test basic ops
}
