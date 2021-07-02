#include "cli/cli.h"
#include "config.h"
#include <CLI/CLI.hpp>
#include <fmt/printf.h>

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace fmt::literals;
    using namespace angonoka::cli;

    register_abort_handlers();

    Options options;
    CLI::App app{
        "Angonoka is a time estimation software based on statistical "
        "modeling.\n",
        ANGONOKA_NAME};

    auto* version = app.add_flag(
        "--version",
        "Display program version information and exit");

    app.add_flag(
        "--color,!--no-color",
        options.color,
        "Force colored output");
    app.add_flag("-q,--quiet", options.quiet, "Give less output");
    app.add_flag("-v,--verbose", options.verbose, "Give more output");
    app.add_option("input file", options.filename)->required();

    const auto version_requested = [&] {
        if (version->count() > 0) {
            fmt::print("{}\n", ANGONOKA_VERSION);
            return true;
        }
        return false;
    };

    try {
        app.parse(argc, argv);
        if (version_requested()) return EXIT_SUCCESS;
        const auto config = parse_config(options);
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const CLI::ParseError& e) {
        if (version_requested()) return EXIT_SUCCESS;
        return app.exit(e);
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
