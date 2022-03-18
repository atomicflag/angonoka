#include <catch2/catch.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include "simulation.h"
#include "predict.h"
#include "config/load.h"
#include "stub/random_utils.h"

namespace angonoka::stun {
float RandomUtils::normal(float min, float max) noexcept {
    return std::midpoint(min,max);
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
        const auto h = histogram(config, schedule);
        auto max_bin
            = std::distance(h.begin(), ranges::max_element(h));

        REQUIRE(h.size() == 121);
        REQUIRE(h.axis().bin(0).lower() == 0.F);
        REQUIRE(h.axis().bin(0).upper() == 60.F);
        // Makespan should be 2 hours,
        // falls into 7200-7299 bin
        REQUIRE(h.axis().bin(max_bin).center() == 7230.F);
    }
}
