#include "config.h"
#include <clipp.h>
#include <fmt/printf.h>
#include <memory>
#include <string>

namespace {
enum class Mode { none, help, version };
struct Options {
    std::string filename;
    Mode mode{Mode::none};
};
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace clipp;

    Options options;

    const auto cli
        = (option("--version").set(options.mode, Mode::version)
               % "Show version",
           option("-h", "--help").set(options.mode, Mode::help)
               % "Show help",
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

    return 0;
}
