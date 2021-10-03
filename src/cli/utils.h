#pragma once

#include "options.h"
#include <cstdio>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
/**
    Print text to stdout if not in quiet mode.

    @param options CLI options
*/
template <typename... T>
void print(
    const Options& options,
    fmt::format_string<T...> fmt,
    T&&... args)
{
    if (options.quiet) return;
    fmt::print(fmt, std::forward<T>(args)...);
}

/**
    Prints red text.

    Conditionally disables the color depending on
    CLI options.

    @param options CLI options
*/
template <typename... T>
void print_error(
    const Options& options,
    std::FILE* output,
    fmt::format_string<T...> fmt,
    T&&... args)
{
    if (options.color) {
        fmt::print(
            output,
            fg(fmt::terminal_color::red),
            fmt::string_view{fmt},
            std::forward<T>(args)...);
    } else {
        fmt::print(output, fmt, std::forward<T>(args)...);
    }
}

/**
    Critical error message.

    Used for progress messages with ellipsis like

    Progress message... <die()>Error
    An error has occured.

    @param options CLI options
*/
template <typename... T>
void die(
    const Options& options,
    fmt::format_string<T...> fmt,
    T&&... args)
{

    if (!options.quiet) print_error(options, stdout, "Error\n");
    print_error(options, stderr, fmt, std::forward<T>(args)...);
}
} // namespace angonoka::cli
