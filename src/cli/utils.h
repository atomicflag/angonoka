#pragma once

#include "options.h"
#include <chrono>
#include <fmt/color.h>
#include <fmt/printf.h>

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

// TODO: doc, test, expects
template <typename T> struct humanize {
    T value;
};
template <typename T> humanize(T) -> humanize<T>;

namespace detail {
    // TODO: doc, test, expects
    template <typename T> auto format_duration(auto name)
    {
        return [=](bool& needs_space,
                   std::chrono::seconds& total,
                   auto& ctx) {
            const auto dur = std::chrono::floor<T>(total);
            if (dur == dur.zero()) return;
            total -= dur;
            if (needs_space) fmt::format_to(ctx.out(), " ");
            const auto ticks = dur.count();
            fmt::format_to(
                ctx.out(),
                ticks == 1 ? "{} {}" : "{} {}s",
                ticks,
                name);
            needs_space = true;
        };
    }

    // TODO: doc, test, expects
    void
    compose_duration_fmts(const auto& obj, auto& ctx, auto&&... fns)
    {
        bool needs_space = false;
        auto total = std::chrono::duration_cast<std::chrono::seconds>(
            obj.value);
        (fns(needs_space, total, ctx), ...);
    }
} // namespace detail
} // namespace angonoka::cli

namespace fmt {
using angonoka::cli::humanize;
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
        if (obj.value == decltype(obj.value)::zero())
            return format_to(ctx.out(), "0 seconds");
        // TODO: refactor?
        compose_duration_fmts(
            obj,
            ctx,
            format_duration<std::chrono::months>("month"),
            format_duration<std::chrono::days>("day"),
            format_duration<std::chrono::hours>("hour"),
            format_duration<std::chrono::minutes>("minute"),
            format_duration<std::chrono::seconds>("second"));
        return ctx.out();
    }
};
} // namespace fmt
