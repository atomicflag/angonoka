#include "config.h"
#include "config/load.h"
#include "exceptions.h"
#include "predict.h"
#include <boost/hana/functional/overload.hpp>
#include <clipp.h>
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
enum class Mode { None, Help, Version };

// TODO: doc, test, expects
bool output_is_terminal() noexcept
{
    return isatty(fileno(stdout)) == 1;
}

// TODO: doc, test, expects
struct Options {
    std::string filename;
    Mode mode{Mode::None};
    bool verbose{false};
    bool color{output_is_terminal()};
};

// TODO: doc, test, expects
constexpr auto red_text = [](auto&&... args) {
    fmt::print(
        fg(fmt::terminal_color::red),
        std::forward<decltype(args)>(args)...);
};

// TODO: doc, test, expects
constexpr auto colorless_text = [](auto&&... args) {
    fmt::print(std::forward<decltype(args)>(args)...);
};

// TODO: doc, test, expects
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

// TODO: doc, test, expects
constexpr auto die
    = colorize(red_text, [](auto&& print, auto&&... args) {
          print("Error\n");
          print(std::forward<decltype(args)>(args)...);
      });

// TODO: doc, test, expects
auto on_simple_progress_event(const SimpleProgressEvent& e)
{
    switch (e) {
    case SimpleProgressEvent::ScheduleOptimizationStart:
        fmt::print("Optimizing schedule...\n");
        // TODO: show an indicator
        return;
    case SimpleProgressEvent::ScheduleOptimizationDone:
        fmt::print("Done optimizing schedule.\n");
        return;
    case SimpleProgressEvent::Finished:
        fmt::print("Prediction complete.\n");
        return;
    }
}

// TODO: doc, test, expects
void on_schedule_optimization_event(
    const ScheduleOptimizationEvent& e)
{
    fmt::print("Optimization progress {}\n", e.progress);
    // TODO: update progress bar
}

// TODO: doc, test, expects
bool is_final_event(ProgressEvent& evt) noexcept
{
    if (auto* e = boost::get<SimpleProgressEvent>(&evt))
        return *e == SimpleProgressEvent::Finished;
    return false;
}

// TODO: doc, test, expects
auto make_event_consumer(auto&&... callbacks) noexcept
{
    return [=](Queue<ProgressEvent>& queue,
               std::future<Prediction>& prediction) {
        using namespace std::literals::chrono_literals;
        constexpr auto event_timeout = 100ms;
        ProgressEvent evt;
        while (!is_final_event(evt)) {
            if (!queue.try_dequeue(evt)) {
                prediction.wait_for(event_timeout);
                continue;
            }
            boost::apply_visitor(
                overload(
                    std::forward<decltype(callbacks)>(callbacks)...),
                evt);
        }
    };
}

// TODO: doc, test, expects
void run_prediction(const Configuration& config)
{
    auto [prediction_future, event_queue] = predict(config);
    auto consumer = make_event_consumer(
        on_simple_progress_event,
        on_schedule_optimization_event);
    consumer(*event_queue, prediction_future);
    prediction_future.get();
}

// TODO: doc, test, expects
void parse_config(const Options& options)
{
    fmt::print("Parsing configuration... ");
    try {
        const auto config = load_file(options.filename);
        fmt::print("OK\n");
        run_prediction(config);
    } catch (const YAML::BadFile& e) {
        die(options,
            "Error reading tasks and agents from file \"{}\".\n",
            options.filename);
        throw;
    } catch (const ValidationError& e) {
        die(options, "Validation error: {}\n", e.what());
        throw;
    } catch (const std::runtime_error& e) {
        die(options, "Unexpected error: {}\n", e.what());
        throw;
    }
}

// TODO: doc, test, expects
void help_and_version(Mode mode, const clipp::man_page& man_page)
{
    switch (mode) {
    case Mode::Help: fmt::print("{}", man_page); return;
    case Mode::Version:
        fmt::print(
            "{} version {}\n",
            ANGONOKA_NAME,
            ANGONOKA_VERSION);
        return;
    default:;
    }
}
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    using namespace clipp;

    Options options;

    const auto cli
        = (option("--version").set(options.mode, Mode::Version)
               % "Print the version number and exit.",
           option("-h", "--help").set(options.mode, Mode::Help)
               % "Print usage and exit.",
           option("-v", "--verbose").set(options.verbose)
               % "Print debugging messages about prediction "
                 "progress.",
           option("--color").set(options.color)
               % "Force colored output.", // TODO: add no-color
           value("input file", options.filename));

    const auto man_page = make_man_page(cli, ANGONOKA_NAME);
    const auto cli_parse = parse(argc, argv, cli);
    if (options.mode != Mode::None) {
        help_and_version(options.mode, man_page);
        return 0;
    }
    if (cli_parse.any_error()) {
        fmt::print("{}", man_page);
        return 1;
    }
    try {
        parse_config(options);
    } catch (...) {
        return 1;
    }
    return 0;
}
