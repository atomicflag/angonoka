#include "config.h"
#include "config/load.h"
#include "exceptions.h"
#include <clipp.h>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <memory>
#include <string>

namespace {
using namespace angonoka;

// TODO: doc, test, expects
enum class Mode { none, help, version };

// TODO: doc, test, expects
struct Options {
    std::string filename;
    Mode mode{Mode::none};
    bool verbose{false};
};

// TODO: doc, test, expects
template <typename... Ts> void die(Ts&&... args)
{
    // TODO: detect if color can be used
    fmt::print(fg(fmt::terminal_color::red), "Error\n");
    fmt::print(
        fg(fmt::terminal_color::red),
        std::forward<Ts>(args)...);
}

// TODO: doc, test, expects
int run(const Options& options)
{
    fmt::print("Parsing configuration... ");
    try {
        const auto config = load_file(options.filename);
        fmt::print("OK\n");
    } catch (const YAML::BadFile& e) {
        die("Error reading tasks and agents from file \"{}\".\n",
            options.filename);
        return 1;
    } catch (const ValidationError& e) {
        die("Validation error: {}\n", e.what());
        // TODO: Add more context to errors (see TODO in exceptions.h)
        return 1;
    } catch (const std::runtime_error& e) {
        die("Unexpected error: {}\n", e.what());
        return 1;
    }
    return 0;
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

    const auto print_help = [&] {
        fmt::print("{}", make_man_page(cli, ANGONOKA_NAME));
    };
    const auto cli_parse = parse(argc, argv, cli);

    switch (options.mode) {
    case Mode::help: print_help(); return 0;
    case Mode::version:
        fmt::print(
            "{} version {}\n",
            ANGONOKA_NAME,
            ANGONOKA_VERSION);
        return 0;
    default:;
    }
    if (cli_parse.any_error()) {
        print_help();
        return 1;
    }
    return run(options);
}
