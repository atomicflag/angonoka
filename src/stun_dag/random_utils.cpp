#include "random_utils.h"

namespace angonoka::stun_dag {
RandomUtils::RandomUtils() = default;

RandomUtils::RandomUtils(gsl::index seed)
    : generator{seed}
{
}

float RandomUtils::get_uniform() noexcept
{
    return uniform(generator);
}

int RandomUtils::get_uniform_int(int16 max) noexcept
{
    using param_type = decltype(uniform_int)::param_type;
    uniform_int.param(param_type{0, max});
    return uniform_int(generator);
}
} // namespace angonoka::stun_dag
