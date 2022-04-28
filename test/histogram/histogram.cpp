#include "config/load.h"
#include "predict.h"
#include "simulation.h"
#include "stub/random_utils.h"
#include <catch2/catch.hpp>
#include <range/v3/algorithm/max_element.hpp>

namespace angonoka::stun {
float RandomUtils::normal(float min, float max) noexcept
{
    return std::midpoint(min, max);
}
} // namespace angonoka::stun

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
        const OptimizedSchedule schedule{
            .schedule{{0, 0}, {1, 1}},
            .makespan{std::chrono::seconds{7230}}};
        const auto h = histogram(config, schedule);
        const int max_bin = static_cast<int>(
            std::distance(h.begin(), ranges::max_element(h)));

        REQUIRE(h.size() == 121);
        REQUIRE(h.axis().bin(0).lower() == 0.F);
        REQUIRE(h.axis().bin(0).upper() == 60.F);
        // Makespan should be 2 hours,
        // falls into 7200-7299 bin
        REQUIRE(h.axis().bin(max_bin).center() == 7230.F);
    }
}

TEST_CASE("histogram stats")
{
    using namespace angonoka;
    using namespace std::chrono_literals;

    SECTION("regular")
    {
        Histogram hist{{{1, 0.F, 10.F}}};

        hist(1.F);
        hist(101.F);
        hist(101.F);
        hist(201.F);
        hist(201.F);
        hist(201.F);
        hist(301.F);
        hist(301.F);
        hist(401.F);

        const auto s = stats(hist);

        REQUIRE(s.p25 == 105s);
        REQUIRE(s.p50 == 205s);
        REQUIRE(s.p75 == 305s);
        REQUIRE(s.p95 == 405s);
        REQUIRE(s.p99 == 405s);
    }

    SECTION("rounding bug")
    {
        Histogram hist{{{1, 0.F, 11.F}}};

        hist(1.F);

        const auto s = stats(hist);
        REQUIRE(s.p25 == 6s);
        REQUIRE(s.p50 == 6s);
        REQUIRE(s.p75 == 6s);
        REQUIRE(s.p95 == 6s);
        REQUIRE(s.p99 == 6s);
    }
}
