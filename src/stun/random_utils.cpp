#include "random_utils.h"

namespace angonoka::stun {
RandomUtils::RandomUtils() = default;

RandomUtils::RandomUtils(gsl::index seed)
    : generator{seed}
{
}

float RandomUtils::uniform_01() noexcept
{
    return uniform_01_(generator);
}

int16 RandomUtils::uniform_int(int16 max) noexcept
{
    Expects(max >= 0);
    using param_type = decltype(uniform_int_)::param_type;
    uniform_int_.param(param_type{0, max});
    return uniform_int_(generator);
}
} // namespace angonoka::stun
