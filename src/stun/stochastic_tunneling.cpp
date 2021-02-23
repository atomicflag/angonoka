#include "stochastic_tunneling.h"
#include "random_utils.h"
#include "schedule_info.h"
#include "temperature.h"
#include "utils.h"
#include <range/v3/algorithm/copy.hpp>

namespace {
using namespace angonoka::stun;

/**
    Adjusts the energy to be within 0.0 to 1.0 range.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    @param lowest_e Lowest energy
    @param energy   Target energy
    @param gamma    Tunneling parameter

    @return STUN-adjusted energy
*/
float stun_fn(float lowest_e, float energy, float gamma) noexcept
{
    Expects(lowest_e >= 0.F);
    Expects(energy >= lowest_e);

    const auto result = 1.F - std::exp(-gamma * (energy - lowest_e));

    Ensures(result >= 0.F && result <= 1.F);
    return result;
}

} // namespace

namespace angonoka::stun {

void StochasticTunneling::get_new_neighbor() noexcept
{
    Expects(!current_state.empty());
    Expects(!target_state.empty());

    ranges::copy(current_state, target_state.begin());
    (*mutator)(target_state);
    target_e = (*makespan)(target_state);

    Ensures(target_e >= 0.F);
}

bool StochasticTunneling::neighbor_is_better() noexcept
{
    Expects(target_e >= 0.F);
    Expects(current_e >= 0.F);
    Expects(lowest_e >= 0.F);
    Expects(current_e >= lowest_e);
    Expects(!target_state.empty());
    Expects(!current_state.empty());
    Expects(!best_state.empty());

    if (target_e < current_e) {
        if (target_e < lowest_e) {
            lowest_e = target_e;
            ranges::copy(target_state, best_state.begin());
            current_s = stun_fn(lowest_e, current_e, gamma);
        }
        std::swap(current_state, target_state);
        current_e = target_e;
        return true;
    }
    return false;
}

void StochasticTunneling::perform_stun() noexcept
{
    Expects(target_e >= 0.F);
    Expects(lowest_e >= 0.F);
    Expects(current_s >= 0.F);
    Expects(!target_state.empty());
    Expects(!current_state.empty());

    target_s = stun_fn(lowest_e, target_e, gamma);
    const auto delta_s = target_s - current_s;
    const auto pr = std::min(1.F, std::exp(-*temp * delta_s));
    if (pr >= random->uniform_01()) {
        std::swap(current_state, target_state);
        current_e = target_e;
        current_s = target_s;
        temp->update(target_s);
    }

    Ensures(target_s >= 0.F);
    Ensures(current_s >= 0.F);
}

void StochasticTunneling::update() noexcept
{
    Expects(!state_buffer.empty());
    Expects(!current_state.empty());
    get_new_neighbor();
    if (neighbor_is_better()) return;
    perform_stun();
}

void StochasticTunneling::init_energies()
{
    Expects(!current_state.empty());

    current_e = (*makespan)(current_state);
    lowest_e = current_e;
    current_s = stun_fn(lowest_e, current_e, gamma);

    Ensures(current_e >= 0.F);
    Ensures(lowest_e >= 0.F);
    Ensures(current_s >= 0.F && current_s <= 1.F);
}

void StochasticTunneling::prepare_state_spans(index state_size)
{
    Expects(state_size > 0);

    state_buffer.resize(static_cast<gsl::index>(state_size) * 3);
    auto* data = state_buffer.data();
    const auto next = [&] {
        return std::exchange(data, std::next(data, state_size));
    };
    best_state = {next(), state_size};
    current_state = {next(), state_size};
    target_state = {next(), state_size};

    Ensures(current_state.size() == state_size);
    Ensures(target_state.size() == state_size);
    Ensures(best_state.size() == state_size);
}

void StochasticTunneling::init_states(State source_state) const
{
    Expects(source_state.size() == best_state.size());
    Expects(source_state.size() == current_state.size());
    Expects(!source_state.empty());

    ranges::copy(source_state, best_state.begin());
    ranges::copy(source_state, current_state.begin());
}

void StochasticTunneling::reset(State state)
{
    Expects(!state.empty());
    prepare_state_spans(state.size());
    init_states(state);
    init_energies();
}

State StochasticTunneling::state() const { return best_state; }

float StochasticTunneling::energy() const { return lowest_e; }

StochasticTunneling::StochasticTunneling(const STUNOptions& options)
    : mutator{options.mutator}
    , random{options.random}
    , makespan{options.makespan}
    , temp{options.temp}
    , gamma{options.gamma}
{
}

StochasticTunneling::StochasticTunneling(
    const STUNOptions& options,
    State state)
    : StochasticTunneling{options}
{
    Expects(!state.empty());
    reset(state);
}
} // namespace angonoka::stun
