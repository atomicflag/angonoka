#include "stochastic_tunneling.h"
#include "random_utils.h"
#include "temperature.h"
#include "utils.h"

namespace {
using namespace angonoka::stun_dag;

/**
    Adjusts the energy to be within 0.0 to 1.0 range.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    @param lowest_e Lowest energy
    @param energy   Target energy
    @param gamma    Tunneling parameter

    @return STUN-adjusted energy
*/
/* float stun(float lowest_e, float energy, float gamma) noexcept
{
    Expects(lowest_e >= 0.F);
    Expects(energy >= lowest_e);

    const auto result = 1.F - std::exp(-gamma * (energy - lowest_e));

    Ensures(result >= 0.F && result <= 1.F);
    return result;
} */

/**
    TODO: doc, implement
*/
struct StochasticTunnelingOp {

    [[nodiscard]] STUNResult operator()(State /* state */)
    {
        return {};
    }
};
} // namespace

namespace angonoka::stun_dag {
STUNResult stochastic_tunneling(
    State state,
    const STUNOptions& /* options */) noexcept
{
    StochasticTunnelingOp op;
    return op(state);
}
} // namespace angonoka::stun_dag
