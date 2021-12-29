#include "cli/cli.h"
#include "cli/json_schedule.h"
#include "config.h"
#include <CLI/CLI.hpp>
#include <fmt/printf.h>

namespace {
using namespace angonoka::cli;

void optimization_options(CLI::App& cli)
{
    using Params = angonoka::OptimizationParameters;

    cli.add_flag("--batch-size", "Optimization batch size")
        ->default_val(Params::default_batch_size);
    cli.add_flag("--max-idle-iters", "Optimization halting condition")
        ->default_val(Params::default_max_idle_iters);
    cli.add_flag(
           "--beta-scale",
           "Optimization temperature parameter inertia")
        ->default_val(Params::default_beta_scale);
    cli.add_flag(
           "--stun-window",
           "Optimization temperature adjustment window")
        ->default_val(Params::default_stun_window);
    cli.add_flag("--gamma", "Optimization STUN parameter")
        ->default_val(Params::default_gamma);
    cli.add_flag(
           "--restart-period",
           "Optimization temperature volatility period")
        ->default_val(Params::default_restart_period);
    // TODO: Add validation
}

void common_options(CLI::App& cli, Options& options)
{
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
}

auto schedule_subcommand(
    CLI::App& cli,
    Options& options,
    CLI::Option_group* default_group)
{
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
    return schedule_cmd;
}

auto default_group(CLI::App& cli, Options& options)
{
    auto* group = cli.add_option_group("Default");
    group->add_option("input file", options.filename)
        ->required()
        ->check(CLI::ExistingFile);
    return group;
}
} // namespace

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
    common_options(cli, options);
    optimization_options(cli);
    cli.require_subcommand(-1);
    auto* default_grp = default_group(cli, options);
    auto* schedule_cmd
        = schedule_subcommand(cli, options, default_grp);

    try {
        CLI11_PARSE(cli, argc, argv);
        auto config = parse_config(options);
        // TODO: add parse_opt_params in src/cli/cli.h
        // parse_opt_params(cli, config.opt_params);

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
