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
        expect(text == "a few seconds");
    };

    "short duration"_test = [] {
        constexpr auto d = humanize{1min + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "about a minute");
    };

    "almost 2 min"_test = [] {
        constexpr auto d = humanize{1min + 50s};
        const auto text = fmt::format("{}", d);
        expect(text == "2 minutes");
    };

    "medium duration"_test = [] {
        constexpr auto d = humanize{1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "about an hour");
    };

    "long duration"_test = [] {
        constexpr auto d = humanize{24h * 12 + 1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "12 days");
    };

    "very long duration"_test = [] {
        constexpr auto d = humanize{24h * 35};
        const auto text = fmt::format("{}", d);
        expect(text == "about a month");
    };

    "0 duration"_test = [] {
        constexpr auto d = humanize{0s};
        const auto text = fmt::format("{}", d);
        expect(text == "a few seconds");
    };
};

suite verbose_duration = [] {
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace angonoka::cli;

    "very short duration"_test = [] {
        constexpr auto d = verbose{5s};
        const auto text = fmt::format("{}", d);
        expect(text == "5s");
    };

    "short duration"_test = [] {
        constexpr auto d = verbose{1min + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "1m 15s");
    };

    "almost 2 min"_test = [] {
        constexpr auto d = verbose{1min + 50s};
        const auto text = fmt::format("{}", d);
        expect(text == "1m 50s");
    };

    "medium duration"_test = [] {
        constexpr auto d = verbose{1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "1h 15s");
    };

    "long duration"_test = [] {
        constexpr auto d = verbose{24h * 12 + 1h + 15s};
        const auto text = fmt::format("{}", d);
        expect(text == "12d 1h 15s");
    };

    "very long duration"_test = [] {
        constexpr auto d = verbose{24h * 35};
        const auto text = fmt::format("{}", d);
        expect(text == "1mo 4d 13h 30m 54s");
    };

    "0 duration"_test = [] {
        constexpr auto d = verbose{0s};
        const auto text = fmt::format("{}", d);
        expect(text == "0s");
    };
};
