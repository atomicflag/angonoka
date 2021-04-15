#include "exp_curve_fitter.h"
#include <cmath>
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun {
void ExpCurveFitter::reset() noexcept
{
    *this = ExpCurveFitter();

    Ensures(xxy == 0);
    Ensures(ylogy == 0);
    Ensures(xy == 0);
    Ensures(xylogy == 0);
    Ensures(sumy == 0);
}

float ExpCurveFitter::operator()(float x, float y) noexcept
{
    // Not strictly necessary
    Expects(x >= 0.F);
    Expects(y >= 0.F);

    const auto xy1 = x * y;
    const auto ylogy1 = y * std::log(y);
    xxy += x * xy1;
    ylogy += ylogy1;
    xy += xy1;
    xylogy += x * ylogy1;
    sumy += y;
    const auto divisor = std::fma(sumy, xxy, -std::pow(xy, 2.F));
    if (divisor == 0.F) return 0.F;
    const auto a = (xxy * ylogy - xy * xylogy) / divisor;
    const auto b = (sumy * xylogy - xy * ylogy) / divisor;
    return std::exp(std::fma(b, x, a));
}
} // namespace angonoka::stun
