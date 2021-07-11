#pragma once

#include "options.h"
#include <chrono>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
/**
    Print text to stdout if not in quiet mode.

    @param options CLI options
*/
void print(const Options& options, auto&&... args)
{
    if (options.quiet) return;
    fmt::print(std::forward<decltype(args)>(args)...);
}

/**
    Prints red text.

    Conditionally disables the color depending on
    CLI options.

    @param options CLI options
*/
void red_text(const Options& options, auto&&... args)
{
    if (options.color) {
        fmt::print(
            fg(fmt::terminal_color::red),
            std::forward<decltype(args)>(args)...);
    } else {
        fmt::print(std::forward<decltype(args)>(args)...);
    }
}

/**
    Critical error message.

    Used for progress messages with ellipsis like

    Progress message... <die()>Error
    An error has occured.

    @param options CLI options
*/
void die(const Options& options, auto&&... args)
{
    if (!options.quiet) red_text(options, "Error\n");
    red_text(options, std::forward<decltype(args)>(args)...);
}

/**
    Pretty-print values.

    This class is mostly used for specializing user-defined
    formatters for the fmt library. We don't want to specialize
    the formatter for the original type because we might not
    always want a human-readable output.

    Instead of specializing fmt::formatter<std::chrono::seconds>
    we specialize fmt::formatter<humanize<std::chrono::seconds>>
    thus not interfering with the original formatter.

    @var value Value to be pretty-printed
*/
template <typename T> struct humanize {
    T value;
};
template <typename T> humanize(T) -> humanize<T>;

namespace detail {
    /**
        Helper function to print durations in human-readable form.

        @param article  "a" or "an"
        @param name     Duration (seconds, minutes, etc)

        @return Formatter function
    */
    template <typename T>
    auto format_duration(gsl::czstring article, gsl::czstring name)
    {
        return [=](auto total, auto& ctx) {
            const auto dur = std::chrono::round<T>(total);
            if (dur == dur.zero()) return false;
            const auto ticks = dur.count();
            if (ticks == 1) {
                fmt::format_to(
                    ctx.out(),
                    "about {} {}",
                    article,
                    name);
            } else {
                fmt::format_to(ctx.out(), "{} {}s", ticks, name);
            }
            return true;
        };
    }
} // namespace detail
} // namespace angonoka::cli

namespace fmt {
using angonoka::cli::humanize;
/**
    User-defined formatter for std::chrono durations.
*/
template <typename... Ts>
struct fmt::formatter<humanize<std::chrono::duration<Ts...>>> {
    using value_type = humanize<std::chrono::duration<Ts...>>;
    static constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }

    template <typename FormatContext>
    constexpr auto format(const value_type& obj, FormatContext& ctx)
    {
        using angonoka::cli::detail::format_duration;
        using namespace std::literals::chrono_literals;
        using namespace std::chrono;
        constexpr auto min_threshold = 5s;
        if (obj.value <= min_threshold)
            return format_to(ctx.out(), "a few seconds");
        [&](auto&&... fns) {
            (fns(obj.value, ctx) || ...);
        }(format_duration<months>("a", "month"),
          format_duration<days>("a", "day"),
          format_duration<hours>("an", "hour"),
          format_duration<minutes>("a", "minute"),
          format_duration<seconds>("a", "second"));
        return ctx.out();
    }
};
} // namespace fmt
