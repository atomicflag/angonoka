#include "optimizer_job.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INT(x) base_value(x)
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INT(x) x
#endif // NDEBUG

namespace angonoka::stun {
OptimizerJob::OptimizerJob(
const Options& options)
    : batch_size{options.batch_size}
    , mutator{*options.params, *options.random}
    , makespan{*options.params}
    , temperature{BetaScale{options.beta_scale}, StunWindow{INT(options.stun_window)}, RestartPeriod{static_cast<std::size_t>(INT(options.restart_period))}}
    , stun{
          {.mutator{&mutator},
           .random{options.random},
           .makespan{&makespan},
           .temp{&temperature},
           .gamma{options.gamma}},
          initial_schedule(*options.params)}
{
    Expects(static_cast<std::int_fast32_t>(options.batch_size) > 0);
}

void OptimizerJob::update() noexcept
{
    Expects(batch_size > 0);

    for (int32 i{0}; i < batch_size; ++i) stun.update();
}

[[nodiscard]] Schedule OptimizerJob::schedule() const
{
    return stun.schedule();
}

[[nodiscard]] float OptimizerJob::normalized_makespan() const
{
    return stun.normalized_makespan();
}

void OptimizerJob::reset()
{
    Expects(batch_size > 0);

    stun.reset(initial_schedule(*mutator.options().params));
    temperature.reset();
}

OptimizerJob::OptimizerJob(const OptimizerJob& other)
    : batch_size{other.batch_size}
    , mutator{other.mutator}
    , makespan{other.makespan}
    , temperature{other.temperature}
    , stun{other.stun}
{
    const auto options = stun.options();
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{options.gamma}});
}

OptimizerJob::OptimizerJob(OptimizerJob&& other) noexcept
    : batch_size{other.batch_size}
    , mutator{std::move(other.mutator)}
    , makespan{std::move(other.makespan)}
    , temperature{std::move(other.temperature)}
    , stun{std::move(other.stun)}
{
    const auto options = stun.options();
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{options.gamma}});
}

OptimizerJob& OptimizerJob::operator=(const OptimizerJob& other)
{
    if (&other == this) return *this;
    *this = OptimizerJob{other};
    return *this;
}

OptimizerJob& OptimizerJob::operator=(OptimizerJob&& other) noexcept
{
    if (&other == this) return *this;
    batch_size = other.batch_size;
    mutator = std::move(other.mutator);
    makespan = std::move(other.makespan);
    temperature = std::move(other.temperature);
    stun = std::move(other.stun);
    const auto options = stun.options();
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{options.gamma}});
    return *this;
}

void OptimizerJob::params(const Params& params)
{

    mutator.options({.params{params.params}, .random{params.random}});
    const auto options = stun.options();
    stun.options(
        {.mutator{&mutator},
         .random{params.random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{options.gamma}});
}

auto OptimizerJob::params() const -> Params
{
    auto [p, r] = mutator.options();
    return {p, r};
}

OptimizerJob::~OptimizerJob() noexcept = default;
} // namespace angonoka::stun

#undef INT
