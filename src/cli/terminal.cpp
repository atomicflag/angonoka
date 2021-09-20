#include "utils.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fmt/printf.h>
#include <indicators/cursor_control.hpp>
#include <indicators/terminal_size.hpp>

extern "C" {
/**
    SIGTERM and SIGINT handler.
*/
void abort_handler(int signal)
{
    constexpr auto posix_offset = 128;
    std::quick_exit(posix_offset + signal);
}

#if defined(__llvm__) && defined(ANGONOKA_COVERAGE)
static int __llvm_profile_runtime; // NOLINT
void __llvm_profile_initialize_file(void); // NOLINT
int __llvm_profile_write_file(void); // NOLINT
#endif
}

namespace {
using namespace angonoka::cli;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
constinit std::atomic_flag cursor_suppressed{};

#if defined(__llvm__) && defined(ANGONOKA_COVERAGE)
void save_coverage()
{
    __llvm_profile_initialize_file();
    __llvm_profile_write_file();
}
#endif

/**
    quick_exit handler.
*/
void at_exit()
{
    if (cursor_suppressed.test()) {
        show_cursor();
        std::fflush(stdout);
    }
#if defined(__llvm__) && defined(ANGONOKA_COVERAGE)
    save_coverage();
#endif
}
} // namespace

namespace angonoka::cli {
void register_abort_handlers()
{
    std::at_quick_exit(at_exit);
#if defined(__llvm__) && defined(ANGONOKA_COVERAGE)
    std::atexit(save_coverage);
#endif
    std::signal(SIGINT, abort_handler);
    std::signal(SIGTERM, abort_handler);
}

void erase_line() { fmt::print("\33[2K"); }

void cursor_up() { fmt::print("\033[A\r"); }

void hide_cursor()
{
    cursor_suppressed.test_and_set();
    indicators::show_console_cursor(false);
}

void show_cursor()
{
    indicators::show_console_cursor(true);
    cursor_suppressed.clear();
}

bool output_is_terminal() noexcept
{
    return isatty(fileno(stdout)) == 1;
}
} // namespace angonoka::cli
