#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

TEST_CASE("Parsing durations")
{
    using angonoka::detail::parse_duration;
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;

    REQUIRE(parse_duration("123 s") == 123s);
    REQUIRE(parse_duration("123    s") == 123s);
    REQUIRE(parse_duration("123 seconds") == 123s);
    REQUIRE(parse_duration("123 h") == 123h);
    REQUIRE(parse_duration("1 h 15 minutes") == 1h + 15min);
    REQUIRE(parse_duration("1 h 1 s") == 1h + 1s);
    REQUIRE(parse_duration("01h15m") == 1h + 15min);
    REQUIRE(parse_duration("1 h 1 m 1 s") == 1h + 1min + 1s);
    REQUIRE(parse_duration("1h 1m 1s") == 1h + 1min + 1s);
    REQUIRE(parse_duration("1 min") == 1min);
    REQUIRE(parse_duration("5 mins") == 5min);
    REQUIRE(parse_duration("12 days 15s") == days{12} + 15s);
    REQUIRE_THROWS_AS(parse_duration(""), angonoka::ValidationError);
    REQUIRE_THROWS_AS(
        parse_duration("asdf"),
        angonoka::ValidationError);
    REQUIRE(parse_duration("5 min 1h") == 5min + 1h);
    REQUIRE(parse_duration("1h 5 min") == 1h + 5min);
    REQUIRE(parse_duration("1h and 5 min") == 1h + 5min);
    REQUIRE_THROWS_AS(
        parse_duration("-5 sec"),
        angonoka::ValidationError);
}
