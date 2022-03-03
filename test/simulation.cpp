#include "simulation.h"
#include "config/load.h"
#include "predict.h"
#include <catch2/catch.hpp>

TEST_CASE("Simulation")
{
    using namespace angonoka;

    // TODO: type traits

    SECTION("running simulation")
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
        stun::RandomUtils random{0};

        detail::Simulation sim{config, schedule, random};
        const auto duration = sim();
    }
}
