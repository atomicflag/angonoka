#include "optimizer_job.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

namespace angonoka::stun {
OptimizerJob::OptimizerJob(
    const ScheduleParams& params,
    BatchSize batch_size)
    : params_{&params}
    , batch_size{static_cast<std::int_fast32_t>(batch_size)}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
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

    stun.reset(initial_schedule(*params_));
    temperature
        = {Beta{initial_beta},
           BetaScale{beta_scale},
           StunWindow{stun_window},
           RestartPeriod{restart_period}};
}

OptimizerJob::OptimizerJob(const OptimizerJob& other)
    : params_{other.params_}
    , batch_size{other.batch_size}
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
    mutator.options({.params{params_}, .random{&random_utils}});
}

OptimizerJob::OptimizerJob(OptimizerJob&& other) noexcept
    : params_{std::move(other.params_)}
    , batch_size{other.batch_size}
    , random_utils{other.random_utils}
    , mutator{std::move(other.mutator)}
    , makespan{std::move(other.makespan)}
    , temperature{std::move(other.temperature)}
    , stun{std::move(other.stun)}
{
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
    mutator.options({.params{params_}, .random{&random_utils}});
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
    params_ = other.params_;
    batch_size = other.batch_size;
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
    mutator.options({.params{params_}, .random{&random_utils}});
    return *this;
}

void OptimizerJob::params(const ScheduleParams& params)
{
    params_ = &params;
    mutator.options({.params{params_}, .random{&random_utils}});
}

const ScheduleParams& OptimizerJob::params() const
{
    return *params_;
}

void OptimizerJob::new_random_seed() { random_utils = {}; }

OptimizerJob::~OptimizerJob() noexcept = default;
} // namespace angonoka::stun
