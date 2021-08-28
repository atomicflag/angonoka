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
    CLI::App cli{
        "Angonoka is a time estimation software based on statistical "
        "modeling.\n",
        ANGONOKA_NAME};
    // auto* app = cli.add_subcommand();
    auto* app = &cli;

    auto* version = app->add_flag(
        "--version",
        "Display program version information and exit");

    app->add_flag(
        "--color,!--no-color",
        options.color,
        "Force colored output");
    app->add_flag("-q,--quiet", options.quiet, "Give less output");
    app->add_flag("-v,--verbose", options.verbose, "Give more output");

    auto* input_file = app->add_option_group("input");
    input_file->add_option("input file", options.filename)->required()->check(CLI::ExistingFile);

    auto* schedule_cmd = cli.add_subcommand("schedule", "Output the schedule in JSON format.");
    schedule_cmd->fallthrough();
    // schedule_cmd->needs(input_file);
    CLI::TriggerOff(schedule_cmd, input_file);
    schedule_cmd->add_option("input file", options.filename)->required()->check(CLI::ExistingFile);
    // TODO: CLI help doesn't look clean

    // TODO: CLI11 supports --version now
    const auto version_requested = [&] {
        if (version->count() > 0) {
            fmt::print("{}\n", ANGONOKA_VERSION);
            return true;
        }
        return false;
    };

    try {
        cli.parse(argc, argv);
        if (version_requested()) return EXIT_SUCCESS;
        const auto config = parse_config(options);
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const CLI::ParseError& e) {
        if (version_requested()) return EXIT_SUCCESS;
        return cli.exit(e);
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
