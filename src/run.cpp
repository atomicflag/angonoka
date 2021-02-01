#include "run.h"
#include "config/load.h"
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

void run(std::string_view tasks_yml)
{
    // TODO: Construct ScheduleInfo from System
    const auto system = load_file(tasks_yml);
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
