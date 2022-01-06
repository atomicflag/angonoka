#include "cli.h"
#include "config/load.h"
#include "events.h"
#include "exceptions.h"
#include "predict.h"
#include "progress.h"
#include "utils.h"

namespace {
using namespace angonoka::cli;
/**
    Pretty-print YAML library exception to stdout.

    @param options  CLI options
    @param e        YAML exception
*/
void print_yaml_error(
    const Options& options,
    const YAML::Exception& e)
{
    print_error(
        options,
        stderr,
        "Error at line {}, column {}: {}\n",
        e.mark.line + 1,
        e.mark.column + 1,
        e.msg);
}

/**
    CLI can't convert Option types to safe_numerics
    so we have to do it manually.

    @param value    Value to assign to
    @param option   CLI Option
*/
template <typename T>
void assign_safe(T& value, const CLI::Option& option)
{
    using boost::safe_numerics::base_type;
    value = option.as<typename base_type<T>::type>();
}
} // namespace

namespace angonoka::cli {
Configuration parse_config(const Options& options)
{
    print(options, "Parsing configuration... ");
    try {
        auto config = load_file(options.filename);
        print(options, "OK\n");
        return config;
    } catch (const YAML::BadFile& e) {
        die(options,
            "Error reading tasks and agents from file \"{}\".\n",
            options.filename);
        throw UserError{};
    } catch (const YAML::ParserException& e) {
        die(options, "Error parsing YAML: ");
        print_yaml_error(options, e);
        throw UserError{};
    } catch (const ValidationError& e) {
        die(options, "Validation error: {}\n", e.what());
        throw UserError{};
    } catch (const std::exception& e) {
        die(options, "Unexpected error: {}\n", e.what());
        throw;
    }
}

void run_prediction(
    const Configuration& config,
    const Options& options)
{
    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());

    auto [prediction_future, event_queue] = predict(config);

    if (!options.quiet) {
        Progress progress;
        if (options.color) progress.emplace<ProgressBar>();
        consume_events(
            *event_queue,
            prediction_future,
            EventHandler{&progress, &options});
    }
    prediction_future.get();
    // TODO: Show simulation results
    fmt::print("Done.\n");
}

void parse_opt_params(
    const OptParams& in_params,
    OptimizationParameters& params)
{
    params.batch_size = in_params.batch_size;
    params.max_idle_iters = in_params.max_idle_iters;
    params.beta_scale = in_params.beta_scale;
    params.stun_window = in_params.stun_window;
    params.gamma = in_params.gamma;
    params.restart_period = in_params.restart_period;
}
} // namespace angonoka::cli
