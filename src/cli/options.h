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
    CLI options.

    @var filename   Path to tasks.yaml file
    @var verbose    Debug messages
    @var color      Colored text
    @var quiet      Suppress output
*/
struct Options {
    std::string filename;
    bool verbose{false};
    bool color{output_is_terminal()};
    bool quiet{false};
    std::string output;
};
} // namespace angonoka::cli
