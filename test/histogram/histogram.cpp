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
    using namespace std::chrono_literals;

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

    SECTION("granularity")
    {
        using angonoka::detail::granularity;
        using std::chrono::days;

        REQUIRE(granularity(1h) == 60);
        REQUIRE(granularity(4h) == 60);
        REQUIRE(granularity(5h) == 3600);
        REQUIRE(granularity(days{12}) == 3600);
        REQUIRE(granularity(days{13}) == 86400);
    }

    SECTION("basic histogram")
    {
        const auto config = load_text(text);
        const OptimizedSchedule schedule{
            .schedule{{0, 0}, {1, 1}},
            .makespan{7230s}};
        const auto h = histogram(config, schedule);
        const int max_bin = static_cast<int>(
            std::distance(h.begin(), ranges::max_element(h)));

        REQUIRE(h.size() == 121);
        REQUIRE(h[0].low == 0);
        REQUIRE(h[0].high == 60);
        // Makespan should be 2 hours,
        // falls into 7200-7299 bin
        REQUIRE(h[max_bin].middle == 7230);
    }

    SECTION("bucket size")
    {
        auto config = load_text(text);
        config.bucket_size = 123s;
        const OptimizedSchedule schedule{
            .schedule{{0, 0}, {1, 1}},
            .makespan{7230s}};
        const auto h = histogram(config, schedule);

        REQUIRE(h[0].high == 123);
    }
}

TEST_CASE("histogram stats")
{
    using namespace angonoka;
    using namespace std::chrono_literals;

    SECTION("regular")
    {
        Histogram hist{10};

        hist(1);
        hist(101);
        hist(101);
        hist(201);
        hist(201);
        hist(201);
        hist(301);
        hist(301);
        hist(401);

        const auto s = stats(hist);

        REQUIRE(s.p25 == 105s);
        REQUIRE(s.p50 == 205s);
        REQUIRE(s.p75 == 305s);
        REQUIRE(s.p95 == 405s);
        REQUIRE(s.p99 == 405s);
    }

    SECTION("rounding bug")
    {
        Histogram hist{11};

        hist(1);

        const auto s = stats(hist);
        REQUIRE(s.p25 == 6s);
        REQUIRE(s.p50 == 6s);
        REQUIRE(s.p75 == 6s);
        REQUIRE(s.p95 == 6s);
        REQUIRE(s.p99 == 6s);
    }
}
