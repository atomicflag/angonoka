#include "cli/cli.h"
#include "cli/json_schedule.h"
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

    cli.set_version_flag(
        "--version",
        ANGONOKA_VERSION,
        "Display program version information and exit");

    cli.add_flag(
        "--color,!--no-color",
        options.color,
        "Force colored output");
    cli.add_flag("-q,--quiet", options.quiet, "Give less output");
    cli.add_flag("-v,--verbose", options.verbose, "Give more output");
    cli.require_subcommand(-1);
    auto* input_file = cli.add_option("input file", options.filename)
                           ->check(CLI::ExistingFile);

    auto* schedule_cmd = cli.add_subcommand(
        "schedule",
        "Output the schedule in JSON format.");
    schedule_cmd->excludes(input_file);
    schedule_cmd
        ->add_option("-o,--output", "Output the schedule to a file")
        ->default_str("schedule.json");
    schedule_cmd->add_option("input file", options.filename)
        ->required()
        ->check(CLI::ExistingFile);

    try {
        CLI11_PARSE(cli, argc, argv);
        const auto config = parse_config(options);

        // schedule subcommand
        if (schedule_cmd->parsed()) {
            options.output = (*schedule_cmd)["-o"]->as<std::string>();
            const auto json = json_schedule(config, options);
            save_json(json, options.output);
            return EXIT_SUCCESS;
        }

        // no subcommand
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
