#include "config.h"
#include "config/load.h"
#include "exceptions.h"
#include <clipp.h>
#include <cstdio>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <memory>
#include <string>
#include <unistd.h>

namespace {
using namespace angonoka;

// TODO: doc, test, expects
enum class Mode { none, help, version };

// TODO: doc, test, expects
bool output_is_terminal() noexcept
{
    return isatty(fileno(stdout)) == 1;
}

// TODO: doc, test, expects
struct Options {
    std::string filename;
    Mode mode{Mode::none};
    bool verbose{false};
    bool color{output_is_terminal()};
};

// TODO: doc, test, expects
constexpr auto red_text = [](auto&&... args) {
    fmt::print(
        fg(fmt::terminal_color::red),
        std::forward<decltype(args)>(args)...);
};

// TODO: doc, test, expects
constexpr auto colorless_text = [](auto&&... args) {
    fmt::print(std::forward<decltype(args)>(args)...);
};

// TODO: doc, test, expects
constexpr auto colorize(auto&& color_fn, auto&& fn)
{
    return [=]<typename... Ts>(const Options& options, Ts&&... args)
    {
        if (options.color) {
            fn(color_fn, std::forward<Ts>(args)...);
        } else {
            fn(colorless_text, std::forward<Ts>(args)...);
        }
    };
}

// TODO: doc, test, expects
constexpr auto die
    = colorize(red_text, [](auto&& print, auto&&... args) {
          print("Error\n");
          print(std::forward<decltype(args)>(args)...);
      });

// TODO: doc, test, expects
void run(const Options& options)
{
    fmt::print("Parsing configuration... ");
    try {
        const auto config = load_file(options.filename);
        fmt::print("OK\n");
    } catch (const YAML::BadFile& e) {
        die(options,
            "Error reading tasks and agents from file \"{}\".\n",
            options.filename);
        throw;
    } catch (const ValidationError& e) {
        die(options, "Validation error: {}\n", e.what());
        throw;
    } catch (const std::runtime_error& e) {
        die(options, "Unexpected error: {}\n", e.what());
        throw;
    }
}

// TODO: doc, test, expects
void handle_modes(Mode mode, const clipp::man_page& man_page)
{
    switch (mode) {
    case Mode::help: fmt::print("{}", man_page); return;
    case Mode::version:
        fmt::print(
            "{} version {}\n",
            ANGONOKA_NAME,
            ANGONOKA_VERSION);
        return;
    default:;
    }
}
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace clipp;

    Options options;

    const auto cli
        = (option("--version").set(options.mode, Mode::version)
               % "Show version.",
           option("-h", "--help").set(options.mode, Mode::help)
               % "Show help.",
           option("-v", "--verbose").set(options.verbose)
               % "Give more information during processing.",
           value("input file", options.filename));

    const auto man_page = make_man_page(cli, ANGONOKA_NAME);
    const auto cli_parse = parse(argc, argv, cli);
    if (options.mode != Mode::none) {
        handle_modes(options.mode, man_page);
        return 0;
    }
    if (cli_parse.any_error()) {
        fmt::print("{}", man_page);
        return 1;
    }
    try {
        run(options);
    } catch (...) {
        return 1;
    }
    return 0;
}
