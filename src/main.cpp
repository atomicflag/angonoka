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

    cli.set_version_flag("--version", ANGONOKA_VERSION,"Display program version information and exit" );

    cli.add_flag(
        "--color,!--no-color",
        options.color,
        "Force colored output");
    cli.add_flag("-q,--quiet", options.quiet, "Give less output");
    cli.add_flag("-v,--verbose", options.verbose, "Give more output");

    auto* subcommands = cli.add_subcommand("");
    // auto* subcommands = cli.add_option_group("General");
    subcommands->add_option("input file", options.filename)->required()->check(CLI::ExistingFile);

    auto* schedule_cmd = subcommands->add_subcommand("schedule", "Output the schedule in JSON format.");
    schedule_cmd->fallthrough();
    // CLI::TriggerOff(schedule_cmd, input_file);
    // schedule_cmd->add_option("input file", options.filename)->required()->check(CLI::ExistingFile);
    // TODO: CLI help doesn't look clean

    try {
        cli.parse(argc, argv);
        const auto config = parse_config(options);
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const CLI::ParseError& e) {
        return cli.exit(e);
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
