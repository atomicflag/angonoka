#include "config/load.h"
#include "exceptions.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite parsing_durations = [] {
    using angonoka::detail::parse_duration;
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;

    expect(parse_duration("123 s") == 123s);
    expect(parse_duration("123    s") == 123s);
    expect(parse_duration("123 seconds") == 123s);
    expect(parse_duration("123 h") == 123h);
    expect(parse_duration("1 h 15 minutes") == 1h + 15min);
    expect(parse_duration("1 h 1 s") == 1h + 1s);
    expect(parse_duration("01h15m") == 1h + 15min);
    expect(parse_duration("1 h 1 m 1 s") == 1h + 1min + 1s);
    expect(parse_duration("1h 1m 1s") == 1h + 1min + 1s);
    expect(parse_duration("1 min") == 1min);
    expect(parse_duration("5 mins") == 5min);
    expect(parse_duration("12 days 15s") == days{12} + 15s);
    expect(throws<angonoka::DurationParseError>(
        [&] { parse_duration(""); }));

    expect(throws<angonoka::DurationParseError>(
        [&] { parse_duration("asdf"); }));

    expect(parse_duration("5 min 1h") == 5min + 1h);
    expect(parse_duration("1h 5 min") == 1h + 5min);
    expect(parse_duration("1h and 5 min") == 1h + 5min);
    expect(throws<angonoka::DurationParseError>(
        [&] { parse_duration("-5 sec"); }));
    expect(throws<angonoka::DurationParseError>(
        [&] { parse_duration("s"); }));
};
