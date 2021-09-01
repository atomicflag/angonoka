#include "cli/cli.h"
#include "config.h"
#include <CLI/CLI.hpp>
#include <fmt/printf.h>

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
    cli.add_option("input file", options.filename)->check(CLI::ExistingFile);

    auto* schedule_cmd = cli.add_subcommand("schedule", "Output the schedule in JSON format.");
    schedule_cmd->fallthrough();
    schedule_cmd->add_flag("-o,--output", options.output, "Output the schedule to a file");
    schedule_cmd->add_option("input file", options.filename)->required()->check(CLI::ExistingFile);

    // TODO: CLI help doesn't look clean
    // build/src/angonoka-x86_64 schedule tasks.yml schedule
    // triggers input file in the root CLI :(

    try {
        CLI11_PARSE(cli, argc, argv);
        const auto config = parse_config(options);
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const UserError&) {
        return EXIT_FAILURE;
    } catch (const std::exception&){
        std::terminate();
    }
}
