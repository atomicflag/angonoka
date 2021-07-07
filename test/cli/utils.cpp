#include "cli/utils.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite human_duration = [] {
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace angonoka::cli;

    "very short duration"_test = [] {
        constexpr auto d = humanize{5s};
        const auto text = fmt::format("{}", d);
        expect(text == "5 seconds");
    };

    "short duration"_test = [] {
        constexpr auto d = humanize{1min + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "1 minute 15 seconds");
    };

    "medium duration"_test = [] {
        constexpr auto d = humanize{1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "1 hour 15 seconds");
    };

    "long duration"_test = [] {
        constexpr auto d = humanize{24h * 12 + 1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "12 days 1 hour 15 seconds");
    };

    "very long duration"_test = [] {
        constexpr auto d = humanize{24h * 35};
        const auto text = fmt::format("{}", d);
        expect(
            text == "1 month 4 days 13 hours 30 minutes 54 seconds");
    };

    "0 duration"_test = [] {
        constexpr auto d = humanize{0s};
        const auto text = fmt::format("{}", d);
        expect(text == "0 seconds");
    };
};
