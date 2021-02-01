#include "run.h"
#include <clipp.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>
#include <string>

namespace {
struct CLIOptions {
    std::string tasks_yml{"tasks.yml"};
    bool show_help{false};
};
} // namespace

int main(int argc, char** argv)
{
    using namespace clipp;
    CLIOptions options;

    group cli{
        opt_value("task definition", options.tasks_yml)
            % "YAML tasks configuration",
        option("-h", "--help").set(options.show_help) % "Show help"};

    if (!parse(argc, argv, cli) || options.show_help) {
        fmt::print("{}", make_man_page(cli, *argv));
        return static_cast<int>(!options.show_help);
    }

    angonoka::run(options.tasks_yml);
    return 0;
}
