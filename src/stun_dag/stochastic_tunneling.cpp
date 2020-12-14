#include "stochastic_tunneling.h"
#include "random_utils.h"
#include "temperature.h"
#include "utils.h"

namespace {
using namespace angonoka::stun_dag;

#ifdef UNIT_TEST
constexpr std::uint_fast64_t max_iterations = 2;
#else // UNIT_TEST
constexpr std::uint_fast64_t max_iterations = 100;
#endif // UNIT_TEST

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
    gsl::not_null<const STUNOptions*> options;
    std::uint_fast64_t iteration{0};
    std::vector<StateItem> state_buffer;
    MutState best_state;
    MutState current_state;
    MutState target_state;

    float current_e;
    float lowest_e;
    float target_e;

    float gamma; // TODO: Should be a constant
    float current_s;
    float target_s;

    /**
        TODO: doc
    */
    void run() noexcept
    {
        for (iteration = 0; iteration < max_iterations; ++iteration) {
            // get_new_neighbor();
            // if (neighbor_is_better()) continue;
            // perform_stun();
        }
    }

    /**
        TODO: doc
    */
    [[nodiscard]] STUNResult operator()(State /* state */)
    {
        return {};
    }
};
} // namespace

namespace angonoka::stun_dag {
STUNResult
stochastic_tunneling(State state, const STUNOptions& options) noexcept
{
    StochasticTunnelingOp op{.options{&options}};
    return op(state);
}
} // namespace angonoka::stun_dag
