#include "cli.h"
#include "config/load.h"
#include "events.h"
#include "exceptions.h"
#include "predict.h"
#include "progress.h"
#include "schedule.h"
#include "utils.h"
#include "verbose.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace {
using namespace angonoka::cli;
using namespace angonoka;
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
    Print the histogram quantiles.

    @param options  CLI options
    @param stats    Histogram stats
*/
void print_histogram_stats(
    const Options& options,
    const HistogramStats& stats)
{
    print(
        options,
        "Estimation:\n"
        "  25% chance to complete the project in under {}.\n"
        "  50% chance to complete the project in under {}.\n"
        "  75% chance to complete the project in under {}.\n"
        "  95% chance to complete the project in under {}.\n"
        "  99% chance to complete the project in under {}.\n",
        verbose{stats.p25},
        verbose{stats.p50},
        verbose{stats.p75},
        verbose{stats.p95},
        verbose{stats.p99});
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

nlohmann::json
run_prediction(const Configuration& config, const Options& options)
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
    const auto prediction_result = prediction_future.get();
    print_histogram_stats(options, prediction_result.stats);

    // TODO: WIP
    return detail::to_json(
        config,
        {.schedule{prediction_result.schedule},
         .makespan{prediction_result.makespan}});
}

void parse_opt_params(
    const OptParams& cli_params,
    OptimizationParameters& params)
{
    params.batch_size = cli_params.batch_size;
    params.max_idle_iters = cli_params.max_idle_iters;
    params.beta_scale = cli_params.beta_scale;
    params.stun_window = cli_params.stun_window;
    params.gamma = cli_params.gamma;
    params.restart_period = cli_params.restart_period;
}

void save_prediction_json(
    const nlohmann::json& json,
    const Options& options)
{
    using boost::iostreams::file_descriptor_sink;
    using boost::iostreams::stream;

    Expects(!options.output.empty());
    Expects(!json.empty());

    try {
        stream<file_descriptor_sink> output{options.output};
        output << std::setw(4) << json;
    } catch (const std::runtime_error& e) {
        print_error(
            options,
            stderr,
            "Error saving the prediction results:\n{}",
            e.what());
        throw UserError{};
    }
    print(
        options,
        R"(Time estimation written to "{}")"
        "\n",
        options.output);
}
} // namespace angonoka::cli
