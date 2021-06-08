#include "config.h"
#include "config/load.h"
#include "exceptions.h"
#include "predict.h"
#include <CLI/CLI.hpp>
#include <boost/hana/functional/overload.hpp>
#include <cstdio>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>

namespace {
using namespace angonoka;
using boost::hana::overload;

// TODO: doc, test, expects
struct OperationFailed : std::exception {
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
constexpr auto colorless_text = [](auto&&... args) {
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
            fn(colorless_text, std::forward<Ts>(args)...);
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
    Handle simple progress events.

    TODO: doc, test, expects
*/
auto on_simple_progress_event(auto& progress)
{
    return [=](const SimpleProgressEvent& e) mutable {
        switch (e) {
        case SimpleProgressEvent::ScheduleOptimizationStart:
            fmt::print("Optimizing the schedule...\n");
            progress.start();
            return;
        case SimpleProgressEvent::ScheduleOptimizationDone:
            progress.stop();
            fmt::print("Schedule optimization complete.\n");
            return;
        case SimpleProgressEvent::Finished:
            fmt::print("Probability estimation complete.\n");
            return;
        }
    };
}

/**
    Handle schedule optimization events.

    TODO: doc, test, expects
*/
auto on_schedule_optimization_event(auto& progress)
{
    return [=](const ScheduleOptimizationEvent& e) mutable {
        progress.update(e.progress, "Optimization progress");
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
auto make_event_consumer(auto&&... callbacks) noexcept
{
    return [=](Queue<ProgressEvent>& queue,
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
            visit(
                overload(
                    std::forward<decltype(callbacks)>(callbacks)...),
                evt);
        }
    };
}

// TODO: doc, test, expects
struct TextProgress {
    // TODO: doc, test, expects
    static void start() { }

    // TODO: doc, test, expects
    static void update(float progress, std::string_view message)
    {
        fmt::print("{}: {:.2f}%\n", message, progress * 100.F);
    }

    // TODO: doc, test, expects
    static void stop() { }
};

/**
    Run the prediction algorithm on given configuration.

    @param config Agent and tasks configuration

    TODO: doc, test, expects
*/
void run_prediction(
    const Configuration& config,
    const Options& options)
{
    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());

    const auto run = [=](auto progress) {
        auto [prediction_future, event_queue] = predict(config);
        auto consumer = make_event_consumer(
            on_simple_progress_event(progress),
            on_schedule_optimization_event(progress));
        consumer(*event_queue, prediction_future);
        prediction_future.get();
    };

    if (options.color) {
        // TODO: Implement the progress bar
    } else {
        run(TextProgress{});
    }
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
        throw OperationFailed{};
    } catch (const ValidationError& e) {
        die(options, "Validation error: {}\n", e.what());
        throw OperationFailed{};
    } catch (const std::exception& e) {
        die(options, "Unexpected error: {}\n", e.what());
        throw;
    }
}
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace fmt::literals;

    Options options;
    CLI::App app{
        "Angonoka is a time estimation software based on statistical "
        "modeling.",
        ANGONOKA_NAME};

    auto* version = app.add_flag(
        "--version",
        "Display program version information and exit");

    app.add_flag(
        "-v,--verbose",
        options.verbose,
        "Print debug messages about prediction progress");
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
    } catch (const OperationFailed&) {
        return EXIT_FAILURE;
    }
}
