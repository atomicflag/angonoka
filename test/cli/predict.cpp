#include "cli/cli.h"
#include "config/load.h"
#include <catch2/catch.hpp>

TEST_CASE("JSON prediction")
{
    using namespace angonoka;
    using namespace angonoka::cli::detail;
    using namespace std::chrono_literals;

    SECTION("histogram stats")
    {
        const HistogramStats
            stats{.p25{1s}, .p50{2s}, .p75{3s}, .p95{4s}, .p99{5s}};

        const auto json = to_json(stats);

        REQUIRE(json["p25"] == 1);
        REQUIRE(json["p50"] == 2);
        REQUIRE(json["p75"] == 3);
        REQUIRE(json["p95"] == 4);
        REQUIRE(json["p99"] == 5);
    }

    SECTION("histogram")
    {
        Histogram hist{{{1, 0.F, 10.F}}};

        hist(50.F);
        hist(60.F);
        hist(60.F);
        hist(60.F);
        hist(70.F);
        hist(90.F);
        hist(90.F);

        const auto json = to_json(hist);
        using j = nlohmann::json;

        REQUIRE(json["bin_size"] == 10);
        REQUIRE(json["bins"].size() == 4);
        REQUIRE(json["bins"][0] == j{50, 1});
        REQUIRE(json["bins"][1] == j{60, 3});
        REQUIRE(json["bins"][2] == j{70, 1});
        REQUIRE(json["bins"][3] == j{90, 2});
    }

    SECTION("bin size rounding bug")
    {
        Histogram hist{{{1, 0.F, 3599.9F}}};

        hist(1);
        hist(3700);
        const auto json = to_json(hist);

        REQUIRE(json["bin_size"] == 3600);
        REQUIRE(json["bins"][1][0] == 3600);
    }
}
