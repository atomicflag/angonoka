#include "progress.h"
#include "utils.h"
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
void ProgressText::update(float progress, std::string_view message)
{
    Expects(progress >= 0.F && progress <= 1.F);
    using namespace std::chrono_literals;
    const auto now = clock::now();
    if (now - last_update < 1s) return;
    last_update = now;
    fmt::print("{}: {:.2f}%\n", message, progress * 100.F);
}

int16 terminal_width()
{
    const auto width = indicators::terminal_width();
    constexpr auto min_width = 5;
    constexpr auto max_width = 9999;
    constexpr auto default_width = 50;
    if (width < min_width || width > max_width) return default_width;
    return width;
}

void ProgressBar::start()
{
    hide_cursor();
    bar.set_progress(0);
}

void ProgressBar::update(
    float progress,
    std::string_view /* message */)

{
    bar.set_progress(progress * 100.F);
}

void ProgressBar::stop()
{
    show_cursor();
    erase_line();
}

void start(Progress& p)
{
    constexpr auto visitor = [](auto& v) {
        if constexpr (requires { v.start(); }) v.start();
    };
    boost::variant2::visit(visitor, p);
}

void update(Progress& p, float progress, std::string_view message)
{
    const auto visitor
        = [&](auto& v) { v.update(progress, message); };
    boost::variant2::visit(visitor, p);
}

void stop(Progress& p)
{
    constexpr auto visitor = [](auto& v) {
        if constexpr (requires { v.stop(); }) v.stop();
    };
    boost::variant2::visit(visitor, p);
}
} // namespace angonoka::cli
