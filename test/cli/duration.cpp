#include "cli/humanize.h"
#include "cli/verbose.h"
#include <catch2/catch.hpp>

TEST_CASE("human duration")
{
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace angonoka::cli;

    SECTION("very short duration")
    {
        constexpr auto d = humanize{5s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "a few seconds");
    }

    SECTION("15 seconds")
    {
        constexpr auto d = humanize{15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "15 seconds");
    }

    SECTION("short duration")
    {
        constexpr auto d = humanize{1min + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "about a minute");
    }

    SECTION("almost 2 min")
    {
        constexpr auto d = humanize{1min + 50s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "2 minutes");
    }

    SECTION("medium duration")
    {
        constexpr auto d = humanize{1h + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "about an hour");
    }

    SECTION("2 hours")
    {
        constexpr auto d = humanize{2h};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "2 hours");
    }

    SECTION("a day")
    {
        constexpr auto d = humanize{23h};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "about a day");
    }

    SECTION("long duration")
    {
        constexpr auto d = humanize{24h * 12 + 1h + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "12 days");
    }

    SECTION("very long duration")
    {
        constexpr auto d = humanize{24h * 35};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "about a month");
    }

    SECTION("2 months")
    {
        constexpr auto d = humanize{24h * 70};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "2 months");
    }

    SECTION("0 duration")
    {
        constexpr auto d = humanize{0s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "a few seconds");
    }
}

TEST_CASE("verbose duration")
{
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace angonoka::cli;

    SECTION("very short duration")
    {
        constexpr auto d = verbose{5s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "5s");
    }

    SECTION("short duration")
    {
        constexpr auto d = verbose{1min + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "1m 15s");
    }

    SECTION("almost 2 min")
    {
        constexpr auto d = verbose{1min + 50s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "1m 50s");
    }

    SECTION("medium duration")
    {
        constexpr auto d = verbose{1h + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "1h 15s");
    }

    SECTION("long duration")
    {
        constexpr auto d = verbose{24h * 12 + 1h + 15s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "12d 1h 15s");
    }

    SECTION("very long duration")
    {
        constexpr auto d = verbose{24h * 35};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "1mo 4d 13h 30m 54s");
    }

    SECTION("0 duration")
    {
        constexpr auto d = verbose{0s};
        const auto text = fmt::format("{}", d);
        REQUIRE(text == "0s");
    }
}
