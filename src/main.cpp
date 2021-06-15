#include "config.h"
#include "config/load.h"
#include "exceptions.h"
#include "predict.h"
#include <CLI/CLI.hpp>
#include <atomic>
#include <boost/hana/functional/overload.hpp>
#include <csignal>
#include <cstdio>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/terminal_size.hpp>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>

// TODO: refactor into multiple files

extern "C" {
/**
    SIGTERM and SIGINT handler.
*/
void abort_handler(int signal)
{
    constexpr auto posix_offset = 128;
    std::quick_exit(posix_offset + signal);
}
}

namespace {
using namespace angonoka;
using boost::hana::overload;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
constinit std::atomic_flag cursor_suppressed{};

/**
    Erase the current TTY line.
*/
void erase_line() { fmt::print("\33[2K"); }

/**
    Move the TTY cursor up.
*/
void cursor_up() { fmt::print("\033[A\r"); }

/**
    Hide the TTY cursor.

    The cursor state will be restored if the application is
    terminated by SIGTERM or SIGINT.
*/
void hide_cursor()
{
    cursor_suppressed.test_and_set();
    indicators::show_console_cursor(false);
}

/**
    Show the TTY cursor.
*/
void show_cursor()
{
    indicators::show_console_cursor(true);
    cursor_suppressed.clear();
}

/**
    quick_exit handler.
*/
void at_exit()
{
    if (cursor_suppressed.test()) {
        show_cursor();
        std::fflush(stdout);
    }
}

/**
    Generic user error.

    Will abort the CLI with EXIT_FAILURE code.
*/
struct UserError : std::exception {
};

/**
    Test if the output is a terminal.

    @return True if the output is a terminal.
*/
bool output_is_terminal() noexcept
{
    return isatty(fileno(stdout)) == 1;
}

/**
    CLI options.

    @var filename   Path to tasks.yaml file
    @var action     Single-action command
    @var verbose    Debug messages
    @var color      Colored text
*/
struct Options {
    std::string filename;
    bool verbose{false};
    bool color{output_is_terminal()};
};

/**
    Prints red text.
*/
constexpr auto red_text = [](auto&&... args) {
    fmt::print(
        fg(fmt::terminal_color::red),
        std::forward<decltype(args)>(args)...);
};

/**
    Prints colorless text.
*/
constexpr auto default_text = [](auto&&... args) {
    fmt::print(std::forward<decltype(args)>(args)...);
};

/**
    Decorates a function with a specified color.

    Falls back to colorless text when the colored output isn't
    available.

    @param color_fn Color function (red_text, etc)
    @param fn       Function that prints the text

    @return A function, decorated with the specified color.
*/
constexpr auto colorize(auto&& color_fn, auto&& fn) noexcept
{
    return [=]<typename... Ts>(const Options& options, Ts&&... args)
    {
        if (options.color) {
            fn(color_fn, std::forward<Ts>(args)...);
        } else {
            fn(default_text, std::forward<Ts>(args)...);
        }
    };
}

/**
    Critical error message.
*/
constexpr auto die
    = colorize(red_text, [](auto&& print, auto&&... args) {
          print("Error\n");
          print(std::forward<decltype(args)>(args)...);
      });

/**
    Progress updates for non-TTY outputs.
*/
struct ProgressText {
    /**
        Print the progress update.

        @param progress Progress value from 0.0 to 1.0
        @param message  Status message
    */
    static void update(float progress, std::string_view message)
    {
        // TODO: Throttle updates
        Expects(progress >= 0.F && progress <= 1.F);
        fmt::print("{}: {:.2f}%\n", message, progress * 100.F);
    }
};

/**
    Graphical progress bar for TTY outputs.
*/
struct ProgressBar {
    /**
        Initialize the progress bar.

        TODO: Expects, test
    */
    void start()
    {
        hide_cursor();
        bar.set_progress(0);
    }

    /**
        Update the progress bar.

        @param progress Progress value from 0.0 to 1.0
        @param message  Status message (Unused)

        TODO: Expects, test
    */
    void update(float progress, std::string_view /* message */)

    {
        bar.set_progress(progress * 100.F);
    }

    /**
        Remove the progress bar.

        TODO: Expects, test
    */
    static void stop()
    {
        show_cursor();
        erase_line();
    }

    static constexpr auto padding = 9;
    indicators::BlockProgressBar bar{indicators::option::BarWidth{
        indicators::terminal_width() - padding}};
};

/**
    Text or graphical progress depending on TTY.
*/
using Progress = boost::variant2::variant<ProgressText, ProgressBar>;

/**
    Initiate the progress output.

    @param p Text or graphical progress
*/
void start(Progress& p)
{
    constexpr auto visitor = [](auto& v) {
        if constexpr (requires { v.start(); }) v.start();
    };
    boost::variant2::visit(visitor, p);
}

/**
    Update the progress.

    @param p Text or graphical progress
*/
void update(Progress& p, auto&&... args)
{
    const auto visitor = [&](auto& v) {
        // WTF? I think clang-tidy is having a stroke
        // NOLINTNEXTLINE(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        v.update(std::forward<decltype(args)>(args)...);
    };
    boost::variant2::visit(visitor, p);
}

/**
    Finalize the progress output.

    @param p Text or graphical progress
*/
void stop(Progress& p)
{
    constexpr auto visitor = [](auto& v) {
        if constexpr (requires { v.stop(); }) v.stop();
    };
    boost::variant2::visit(visitor, p);
}

/**
    Handle simple progress events.

    @param progress Text or graphical progress bar

    TODO: doc

    @return SimpleProgressEvent message handler.
*/
auto on_simple_progress_event(
    Progress& progress,
    const Options& options)
{
    return [&](const SimpleProgressEvent& e) mutable {
        switch (e) {
        case SimpleProgressEvent::ScheduleOptimizationStart:
            fmt::print("Optimizing the schedule...\n");
            start(progress);
            return;
        case SimpleProgressEvent::ScheduleOptimizationDone:
            stop(progress);
            if (options.color) {
                cursor_up();
                fmt::print("Optimizing the schedule... OK\n");
            } else {
                fmt::print("Schedule optimization complete.\n");
            }
            return;
        case SimpleProgressEvent::Finished:
            fmt::print("Probability estimation complete.\n");
            return;
        }
    };
}

/**
    Handle schedule optimization events.

    @param progress Text or graphical progress bar

    TODO: test

    @return ScheduleOptimizationEvent message handler.
*/
auto on_schedule_optimization_event(Progress& progress)
{
    return [&](const ScheduleOptimizationEvent& e) mutable {
        Expects(e.progress >= 0.F && e.progress <= 1.F);

        update(progress, e.progress, "Optimization progress");
    };
}

/**
    Check if an event is the final one.

    @param evt Event

    @return True if this is the final event.
*/
bool is_final_event(ProgressEvent& evt) noexcept
{
    using boost::variant2::get_if;
    if (auto* e = get_if<SimpleProgressEvent>(&evt))
        return *e == SimpleProgressEvent::Finished;
    return false;
}

/**
    Compose callbacks into an event consumer function.

    @return Event consumer function.
*/
template <typename... Ts>
auto make_event_consumer(Ts&&... callbacks) noexcept
{
    return [&... callbacks = std::forward<Ts>(callbacks)](
               Queue<ProgressEvent>& queue,
               std::future<Prediction>& prediction) mutable {
        using namespace std::literals::chrono_literals;
        using boost::variant2::visit;
        constexpr auto event_timeout = 100ms;
        ProgressEvent evt;
        while (!is_final_event(evt)) {
            if (!queue.try_dequeue(evt)) {
                prediction.wait_for(event_timeout);
                continue;
            }
            visit(overload(callbacks...), evt);
        }
    };
}

/**
    Run the prediction algorithm on given configuration.

    @param config   Agent and tasks configuration
    @param options  CLI options

    TODO: test
*/
void run_prediction(
    const Configuration& config,
    const Options& options)
{
    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());

    Progress progress;
    if (options.color) progress.emplace<ProgressBar>();

    auto [prediction_future, event_queue] = predict(config);
    auto consumer = make_event_consumer(
        on_simple_progress_event(progress, options),
        on_schedule_optimization_event(progress));
    consumer(*event_queue, prediction_future);
    prediction_future.get();
}

/**
    Parse the configuration YAML.

    @param options CLI options

    @return Tasks and agents.
*/
auto parse_config(const Options& options)
{
    fmt::print("Parsing configuration... ");
    try {
        auto config = load_file(options.filename);
        fmt::print("OK\n");
        return config;
    } catch (const YAML::BadFile& e) {
        die(options,
            "Error reading tasks and agents from file \"{}\".\n",
            options.filename);
        throw UserError{};
    } catch (const ValidationError& e) {
        die(options, "Validation error: {}\n", e.what());
        throw UserError{};
    } catch (const std::exception& e) {
        die(options, "Unexpected error: {}\n", e.what());
        throw;
    }
}

// TODO: doc, test, expects
void register_abort_handlers()
{
    std::at_quick_exit(at_exit);
    std::signal(SIGINT, abort_handler);
    std::signal(SIGTERM, abort_handler);
}
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace fmt::literals;

    register_abort_handlers();

    Options options;
    CLI::App app{
        "Angonoka is a time estimation software based on statistical "
        "modeling.",
        ANGONOKA_NAME};

    auto* version = app.add_flag(
        "--version",
        "Display program version information and exit");

    app.add_flag(
        "--color,!--no-color",
        options.color,
        "Force colored output");
    app.add_option("input file", options.filename)->required();

    const auto version_requested = [&] {
        if (version->count() > 0) {
            fmt::print("{}\n", ANGONOKA_VERSION);
            return true;
        }
        return false;
    };

    try {
        app.parse(argc, argv);
        if (version_requested()) return 0;
        const auto config = parse_config(options);
        run_prediction(config, options);
        return EXIT_SUCCESS;
    } catch (const CLI::ParseError& e) {
        if (version_requested()) return 0;
        return app.exit(e);
    } catch (const UserError&) {
        return EXIT_FAILURE;
    }
}
