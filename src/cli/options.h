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

// TODO: doc, test, expects
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
};
} // namespace angonoka::cli
