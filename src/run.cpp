#include "run.h"
#include "config/load.h"
#include "stun/common.h"
#include "stun/random_utils.h"
#include "stun/schedule_params.h"
#include "stun/stochastic_tunneling.h"
#include "stun/temperature.h"
#include "stun/utils.h"
#include <fmt/ranges.h>
#include <gsl/gsl-lite.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/terminal_size.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>
#include <vector>

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(base_value(x))
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(x)
#endif // NDEBUG

namespace angonoka {
using namespace angonoka::stun;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

// TODO: doc
enum class BatchSize : std::int_fast32_t;

// TODO: doc
enum class MaxIdleIters : std::int_fast32_t;

/**
    Optimization algorithm based on stochastic tunneling.
*/
class Optimizer {
public:
    /**
        Constructor.

        TODO: doc, expects

        @param params Scheduling parameters
        @param batch_size
        @param max_idle_iters
    */
    Optimizer(
        const ScheduleParams& params,
        BatchSize batch_size,
        MaxIdleIters max_idle_iters)
        : params{&params}
        , batch_size{static_cast<std::int_fast32_t>(batch_size)}
        , max_idle_iters{
              static_cast<std::int_fast32_t>(max_idle_iters)}
    {
    }

    // TODO: doc, test, expects
    void update() noexcept
    {
        for (int32 i{0}; i < batch_size; ++i) stun.update();
        idle_iters += batch_size;
        if (stun.energy() == last_energy) return;
        last_energy = stun.energy();
        last_progress = estimated_progress();
        idle_iters = 0;
    }

    // TODO: doc, test, expects
    [[nodiscard]] bool is_complete() const noexcept
    {
        return idle_iters >= max_idle_iters;
    }

    // TODO: doc, test, expects
    [[nodiscard]] float estimated_progress() const noexcept
    {
        const auto batch_progress
            = TO_FLOAT(idle_iters) / TO_FLOAT(max_idle_iters);
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        return (batch_progress + last_progress) / 2.F;
    }

    /**
        The best schedule so far.

        TODO: doc, test, expects

        @return A schedule.
    */
    [[nodiscard]] State state() const { return stun.state(); }

    /**
        The best makespan so far.

        TODO: doc, test, expects

        @return Makespan.
    */
    [[nodiscard]] float energy() const { return stun.energy(); }

    /**
        TODO: doc, test, expects
    */
    void reset()
    {
        stun.reset(initial_state(*params));
        temperature
            = {Beta{initial_beta},
               BetaScale{beta_scale},
               StunWindow{stun_window},
               RestartPeriod{restart_period}};
    }

    Optimizer(const Optimizer& other)
        : params{other.params}
        , batch_size{other.batch_size}
        , max_idle_iters{other.max_idle_iters}
        , idle_iters{other.idle_iters}
        , last_progress{other.last_progress}
        , last_energy{other.last_energy}
        , random_utils{other.random_utils}
        , mutator{other.mutator}
        , makespan{other.makespan}
        , temperature{other.temperature}
        , stun{other.stun}
    {
        stun.options(
            {.mutator{&mutator},
             .random{&random_utils},
             .makespan{&makespan},
             .temp{&temperature},
             .gamma{gamma}});
    }
    Optimizer(Optimizer&& other) noexcept = default;
    Optimizer& operator=(const Optimizer& other)
    {
        *this = Optimizer{other};
        return *this;
    }
    Optimizer& operator=(Optimizer&& other) noexcept = default;
    ~Optimizer() noexcept = default;

private:
    static constexpr auto beta_scale = 1e-4F;
    static constexpr auto stun_window = 10000;
    static constexpr auto gamma = .5F;
    static constexpr auto restart_period = 1 << 20;
    static constexpr auto initial_beta = 1.0F;

    gsl::not_null<const ScheduleParams*> params;
    int16 batch_size;
    int16 max_idle_iters;
    int16 idle_iters{0};
    float last_progress{0.F};
    float last_energy{0.F};
    RandomUtils random_utils;
    Mutator mutator{*params, random_utils};
    Makespan makespan{*params};
    Temperature temperature{
        Beta{initial_beta},
        BetaScale{beta_scale},
        StunWindow{stun_window},
        RestartPeriod{restart_period}};
    StochasticTunneling stun{
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}},
        initial_state(*params)};
};

/**
    Find the optimal schedule.

    TODO: test, expects

    @param params Instance of ScheduleParams

    @return Optimal schedule.
*/
std::vector<stun::StateItem>
optimize(const stun::ScheduleParams& params)
{
    Expects(!params.agent_performance.empty());
    Expects(!params.task_duration.empty());
    Expects(!params.available_agents.empty());
    Expects(
        params.available_agents.size()
        == params.task_duration.size());

    using namespace angonoka::stun;

    constexpr auto batch_size = 1000;
    constexpr auto max_idle_iters = 100'000;

    Optimizer optimizer{
        params,
        BatchSize{batch_size},
        MaxIdleIters{max_idle_iters}};
    while (!optimizer.is_complete()) optimizer.update();

    // TODO: track progress via progress bar
    // TODO: return the Result, not just the state
    // Add a new class that encapsulates stun and adds
    // stopping condition and has an update method.
    return ranges::to<std::vector<StateItem>>(optimizer.state());
}
#pragma clang diagnostic pop

void run(std::string_view tasks_yml)
{
    using namespace angonoka::stun;

    // TODO: Handle YAML exceptions
    const auto config = load_file(tasks_yml);
    const auto schedule_params = to_schedule_params(config);
    const auto state = optimize(schedule_params);
    fmt::print("{}\n", state);
}
} // namespace angonoka

#undef TO_FLOAT
