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
    return uniform_int_(generator, param_type{0, max});
}

float RandomUtils::normal(float min, float max) noexcept
{
    Expects(max > min);
    Expects(min >= 0);
    using param_type = decltype(normal_)::param_type;
    constexpr auto std_devs = 4.F;
    return normal_(
        generator,
        param_type{std::midpoint(min, max), (max - min) / std_devs});
}
} // namespace angonoka::stun
