#include "simulation.h"
#include <catch2/catch.hpp>
#include "config/load.h"
#include "predict.h"

TEST_CASE("simulation")
{
    SECTION("running simulation") {
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

        const auto config = angonoka::load_text(text);
        const angonoka::OptimizedSchedule schedule{
            .schedule{{0,0},{1,1}}
        };
        angonoka::stun::RandomUtils random{0};

        const auto duration = angonoka::detail::run_simulation(config, schedule, random);
    }
}
