#pragma once

#include "configuration.h"
#include "options.h"
#include <CLI/App.hpp>

namespace angonoka::cli {
/**
    Parse the configuration YAML.

    @param options CLI options

    @return Tasks and agents.
*/
Configuration parse_config(const Options& options);

/**
    Run the prediction algorithm on given configuration.

    @param config   Agent and tasks configuration
    @param options  CLI options
*/
void run_prediction(
    const Configuration& config,
    const Options& options);

/**
    Parse optimization-related CLI parameters.

    All CLI parameters should be validated by CLI itself
    before calling this function, so it simply copies
    parameters from one struct to another.

    @param cli_params   CLI optimization parameters
    @param params       Optimization parameters to be set
*/
void parse_opt_params(
    const OptParams& cli_params,
    OptimizationParameters& params);
} // namespace angonoka::cli
