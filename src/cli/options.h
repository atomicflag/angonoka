#pragma once

#include "terminal.h"
#include <exception>
#include <string>

namespace angonoka::cli {
/**
    Generic user error.

    Will abort the CLI with EXIT_FAILURE code.
*/
struct UserError : std::exception {
};

/**
    CLI optimization parameters.

    A structure to hold parameters to be copied to the
    optimization section of the prediction configuration.

    Can't use safe numerics here because CLI11
    can't assign those.

    Same variables as angonoka::OptimizationParameters.

    @var batch_size     Number of STUN iterations in each update
    @var max_idle_iters Halting condition
    @var beta_scale     Temperature parameter's inertia
    @var stun_window    Temperature adjustment window
    @var gamma          Domain-specific parameter for STUN
    @var restart_period Temperature volatility period
*/
struct OptParams {
    int batch_size;
    int max_idle_iters;
    float beta_scale;
    int stun_window;
    float gamma;
    int restart_period;
};

/**
    CLI options.

    TODO: doc

    @var filename   Path to tasks.yaml file
    @var verbose    Debug messages
    @var color      Colored text
    @var quiet      Suppress output
    @var output     File to write the result to
*/
struct Options {
    std::string filename;
    bool verbose{false};
    bool color{output_is_terminal()};
    bool quiet{false};
    std::string output;
    OptParams opt_params;
    bool log_optimization{false};
};
} // namespace angonoka::cli
