#include "optimizer.h"

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(base_value(x))
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(x)
#endif // NDEBUG

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

namespace angonoka::stun {

Optimizer::Optimizer(
    const ScheduleParams& params,
    BatchSize batch_size,
    MaxIdleIters max_idle_iters)
    : params{&params}
    , batch_size{static_cast<std::int_fast32_t>(batch_size)}
    , max_idle_iters{static_cast<std::int_fast32_t>(max_idle_iters)}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
    Expects(static_cast<std::int_fast32_t>(max_idle_iters) > 0);
}

// TODO: Refactor progress updates
void Optimizer::update() noexcept
{
    Expects(batch_size > 0);

    for (int32 i{0}; i < batch_size; ++i) stun.update();
    idle_iters += batch_size;
    const auto progress
        = TO_FLOAT(idle_iters) / TO_FLOAT(max_idle_iters);
    if (stun.energy() == last_energy) {
        const auto next_expected = exp_curve.at(TO_FLOAT(epochs + 1));
        last_progress = std::min(
            exp_curve.at(TO_FLOAT(epochs) + progress / next_expected),
            1.F);
        return;
    }
    last_energy = stun.energy();
    epochs += 1;
    last_progress
        = std::min(exp_curve(TO_FLOAT(epochs), progress), 1.F);
    idle_iters = 0;

    Ensures(epochs > 0);
    Ensures(last_progress >= 0.F);
}

[[nodiscard]] bool Optimizer::has_converged() const noexcept
{
    Expects(max_idle_iters > 0);

    return idle_iters >= max_idle_iters;
}

[[nodiscard]] float Optimizer::estimated_progress() const noexcept
{
    Expects(idle_iters >= 0);
    Expects(max_idle_iters > 0);

    if (has_converged()) return 1.F;
    constexpr auto warmup_epochs = 5;
    if (epochs < warmup_epochs) return 0.F;
    return last_progress;
}

[[nodiscard]] State Optimizer::state() const { return stun.state(); }

[[nodiscard]] float Optimizer::energy() const
{
    return stun.energy();
}

void Optimizer::reset()
{
    Expects(max_idle_iters > 0);
    Expects(batch_size > 0);

    stun.reset(initial_state(*params));
    temperature
        = {Beta{initial_beta},
           BetaScale{beta_scale},
           StunWindow{stun_window},
           RestartPeriod{restart_period}};
    idle_iters = 0;
    epochs = 0;
    last_progress = 0.F;
    last_energy = 0.F;
    exp_curve.reset();
}

Optimizer::Optimizer(const Optimizer& other)
    : params{other.params}
    , batch_size{other.batch_size}
    , max_idle_iters{other.max_idle_iters}
    , idle_iters{other.idle_iters}
    , epochs{other.epochs}
    , last_progress{other.last_progress}
    , last_energy{other.last_energy}
    , random_utils{other.random_utils}
    , mutator{other.mutator}
    , makespan{other.makespan}
    , temperature{other.temperature}
    , stun{other.stun}
    , exp_curve{other.exp_curve}
{
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

Optimizer::Optimizer(Optimizer&& other) noexcept
    : params{std::move(other.params)}
    , batch_size{other.batch_size}
    , max_idle_iters{other.max_idle_iters}
    , idle_iters{other.idle_iters}
    , epochs{other.epochs}
    , last_progress{other.last_progress}
    , last_energy{other.last_energy}
    , random_utils{other.random_utils}
    , mutator{std::move(other.mutator)}
    , makespan{std::move(other.makespan)}
    , temperature{std::move(other.temperature)}
    , stun{std::move(other.stun)}
    , exp_curve{other.exp_curve}
{
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

Optimizer& Optimizer::operator=(const Optimizer& other)
{
    if (&other == this) return *this;
    *this = Optimizer{other};
    return *this;
}

Optimizer& Optimizer::operator=(Optimizer&& other) noexcept
{
    if (&other == this) return *this;
    params = other.params;
    batch_size = other.batch_size;
    max_idle_iters = other.max_idle_iters;
    idle_iters = other.idle_iters;
    epochs = other.epochs;
    last_progress = other.last_progress;
    last_energy = other.last_energy;
    random_utils = other.random_utils;
    mutator = std::move(other.mutator);
    makespan = std::move(other.makespan);
    temperature = std::move(other.temperature);
    stun = std::move(other.stun);
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
    exp_curve = other.exp_curve;
    return *this;
}

Optimizer::~Optimizer() noexcept = default;
} // namespace angonoka::stun

#pragma clang diagnostic pop

#undef TO_FLOAT
