#pragma once

#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/variant2/variant.hpp>
#include <chrono>
#include <indicators/block_progress_bar.hpp>
#include <indicators/terminal_size.hpp>
#include <string_view>

namespace angonoka::cli {
namespace sn = boost::safe_numerics;
using int16 = sn::safe<std::int_fast16_t>;

/**
    Progress updates for non-TTY outputs.
*/
struct ProgressText {
    using clock = std::chrono::steady_clock;

    /**
        Print the progress update.

        Updates are throttled to be displayed only once per second.

        @param progress Progress value from 0.0 to 1.0
        @param message  Status message
    */
    void update(float progress, std::string_view message);

    clock::time_point last_update;
};

// TODO: doc, test, expects
int16 terminal_width();

/**
    Graphical progress bar for TTY outputs.
*/
struct ProgressBar {
    /**
        Initialize the progress bar.
    */
    void start();

    /**
        Update the progress bar.

        @param progress Progress value from 0.0 to 1.0
        @param message  Status message (Unused)
    */
    void update(float progress, std::string_view /* message */);

    /**
        Remove the progress bar.
    */
    static void stop();

    static constexpr auto padding = 9;
    indicators::BlockProgressBar bar{
        indicators::option::BarWidth{terminal_width() - padding}};
};

/**
    Text or graphical progress depending on TTY.
*/
using Progress = boost::variant2::variant<ProgressText, ProgressBar>;

/**
    Initiate the progress output.

    @param p Text or graphical progress
*/
void start(Progress& p);

/**
    Update the progress.

    @param p Text or graphical progress
    // TODO: doc
*/
void update(Progress& p, float progress, std::string_view message);

/**
    Finalize the progress output.

    @param p Text or graphical progress
*/
void stop(Progress& p);
} // namespace angonoka::cli
