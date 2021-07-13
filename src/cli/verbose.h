#pragma once

#include <chrono>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
// TODO: doc, test, expects
template <typename T> struct verbose {
    T value;
};
template <typename T> verbose(T) -> verbose<T>;

namespace detail {
    // TODO: doc, test, expects
    template <typename T>
    constexpr auto verbose_duration(gsl::czstring name)
    {
        return [=](auto& total, auto& ctx, bool& needs_space) {
            const auto dur = std::chrono::floor<T>(total);
            if (dur == dur.zero()) return;
            total -= dur;
            if (needs_space) fmt::format_to(ctx.out(), " ");
            needs_space = true;
            const auto ticks = dur.count();
            fmt::format_to(ctx.out(), "{}{}", ticks, name);
        };
    }
} // namespace detail
} // namespace angonoka::cli

namespace fmt {
using angonoka::cli::verbose;

// TODO: doc, test, expects
template <typename... Ts>
struct fmt::formatter<verbose<std::chrono::duration<Ts...>>> {
    using value_type = verbose<std::chrono::duration<Ts...>>;
    static constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }

    template <typename FormatContext>
    constexpr auto format(const value_type& obj, FormatContext& ctx)
    {
        using angonoka::cli::detail::verbose_duration;
        using namespace std::chrono;
        if (obj.value == obj.value.zero())
            return format_to(ctx.out(), "0s");
        bool needs_space = false;
        auto total = duration_cast<seconds>(obj.value);
        [&](auto&&... fns) {
            (fns(total, ctx, needs_space), ...);
        }(verbose_duration<months>("mo"),
          verbose_duration<days>("d"),
          verbose_duration<hours>("h"),
          verbose_duration<minutes>("m"),
          verbose_duration<seconds>("s"));
        return ctx.out();
    }
};
} // namespace fmt
