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

    cli.add_flag("--batch-size", "Optimization batch size")->default_str("30000");
    cli.add_flag("--max-idle-iters", "Optimization halting condition")->default_str("1500000");
    cli.add_flag("--beta-scale",  "Optimization temperature parameter inertia")->default_str("1e-4F");
    cli.add_flag("--stun-window", "Optimization temperature adjustment window")->default_str("10000");
    cli.add_flag("--gamma",  "Optimization STUN parameter")->default_str("0.5");
    cli.add_flag("--restart-period",  "Optimization temperature volatility period")->default_str("1048576");
    // TODO: Add validation

    cli.require_subcommand(-1);

    auto* default_group = cli.add_option_group("Default");
    default_group->add_option("input file", options.filename)
        ->required()
        ->check(CLI::ExistingFile);

    auto* schedule_cmd = cli.add_subcommand(
        "schedule",
        "Output the schedule in JSON format.");
    schedule_cmd->excludes(default_group);
    schedule_cmd
        ->add_option("-o,--output", "Output the schedule to a file")
        ->default_str("schedule.json");
    schedule_cmd->add_option("input file", options.filename)
        ->required()
        ->check(CLI::ExistingFile);

    try {
        CLI11_PARSE(cli, argc, argv);
        const auto config = parse_config(options);
        // TODO: Assign optimization parameters

        // schedule subcommand
        if (schedule_cmd->parsed()) {
            options.output = (*schedule_cmd)["-o"]->as<std::string>();
            const auto json = json_schedule(config, options);
            save_json(json, options);
            return EXIT_SUCCESS;
        }

        // no subcommand
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
