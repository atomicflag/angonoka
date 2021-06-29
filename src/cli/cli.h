#pragma once

#include "configuration.h"
#include "options.h"

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
} // namespace angonoka::cli
