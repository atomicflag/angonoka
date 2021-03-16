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

void Optimizer::update() noexcept
{
    Expects(batch_size > 0);

    for (int32 i{0}; i < batch_size; ++i) stun.update();
    idle_iters += batch_size;
    if (stun.energy() == last_energy) return;
    last_energy = stun.energy();
    last_progress = estimated_progress();
    idle_iters = 0;

    Ensures(last_progress > 0.F);
}

[[nodiscard]] bool Optimizer::is_complete() const noexcept
{
    Expects(max_idle_iters > 0);

    return idle_iters >= max_idle_iters;
}

[[nodiscard]] float Optimizer::estimated_progress() const noexcept
{
    Expects(idle_iters >= 0);
    Expects(max_idle_iters > 0);

    const auto remaining = 1.F - last_progress;
    const auto batch_progress
        = TO_FLOAT(idle_iters) / TO_FLOAT(max_idle_iters);
    return last_progress + batch_progress * remaining;
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
}

Optimizer::Optimizer(const Optimizer& other)
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
Optimizer::Optimizer(Optimizer&& other) noexcept = default;
Optimizer& Optimizer::operator=(const Optimizer& other)
{
    *this = Optimizer{other};
    return *this;
}
Optimizer& Optimizer::operator=(Optimizer&& other) noexcept = default;
Optimizer::~Optimizer() noexcept = default;
} // namespace angonoka::stun

#pragma clang diagnostic pop

#undef TO_FLOAT