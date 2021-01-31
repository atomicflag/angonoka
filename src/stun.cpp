#include <clipp.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>
#include <memory>
#include <string>

#include "stun/common.h"
#include "stun/random_utils.h"
#include "stun/schedule_info.h"
#include "stun/stochastic_tunneling.h"
#include "stun/temperature.h"
#include "stun/utils.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <utility>

namespace angonoka {

void run(std::string_view /* tasks_yml */)
{
    // TODO: Move this function into it's own cpp
    // TODO: Parse config into a System
    // TODO: Construct ScheduleInfo from System
    /*
    float beta = 1.0F;
    for (int i{0}; i < 10; ++i) {

        RandomUtils random_utils;
        Mutator mutator{info, random_utils};
        Makespan makespan{info};
        Temperature temperature{
            Beta{beta},
            BetaScale{1e-4f},
            StunWindow{10000}};
        auto r = stochastic_tunneling(
            state,
            STUNOptions{
                .mutator{&mutator},
                .random{&random_utils},
                .makespan{&makespan},
                .temp{&temperature},
                .gamma{.5F}});
        state = std::move(r.state);
        beta = r.temperature;
    }
    fmt::print("{}\n", state);
    */
}
} // namespace angonoka

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
