#include "stochastic_tunneling.h"
#include "common.h"
#include "makespan_estimator.h"
#include "random_utils.h"
#include <cmath>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/to_container.hpp>
#include <utility>

namespace {
using namespace angonoka::stun;
using angonoka::stun::index;

float stun(float lowest_e, float current_e, float gamma) noexcept
{
    Expects(lowest_e >= 0.F);
    Expects(current_e >= lowest_e);

    const auto result
        = 1.F - std::exp(-gamma * (current_e - lowest_e));

    Ensures(result >= 0.F && result <= 1.F);
    return result;
}

struct StochasticTunneling {
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtilsT*> random_utils;
    gsl::not_null<MakespanEstimatorT*> makespan;
    BetaDriver beta_driver;

    std::vector<int16> int_data;
    span<int16> best_state;
    span<int16> current_state;
    span<int16> target_state;

    float current_e;
    float lowest_e;
    float target_e;

    float gamma; // TODO: Should be a constant
    float current_s;
    float target_s;

    std::uint_fast64_t current_iteration{0};

    void get_new_neighbor() noexcept
    {
        Expects(!current_state.empty());
        Expects(!target_state.empty());

        ranges::copy(current_state, target_state.begin());
        random_utils->get_neighbor_inplace(target_state);
        target_e = (*makespan)(target_state);

        Ensures(target_e >= 0.F);
    }

    bool neighbor_is_better() noexcept
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
                current_s = stun(lowest_e, current_e, gamma);
            }
            std::swap(current_state, target_state);
            current_e = target_e;
            return true;
        }
        return false;
    }

    void perform_stun() noexcept
    {
        Expects(target_e >= 0.F);
        Expects(lowest_e >= 0.F);
        Expects(current_s >= 0.F);
        Expects(!target_state.empty());
        Expects(!current_state.empty());

        target_s = stun(lowest_e, target_e, gamma);
        const auto delta_s = target_s - current_s;
        const auto pr
            = std::min(1.F, std::exp(-beta_driver.beta() * delta_s));
        if (pr >= random_utils->get_uniform()) {
            std::swap(current_state, target_state);
            current_e = target_e;
            current_s = target_s;
            beta_driver.update(target_s, current_iteration);
        }

        Ensures(target_s >= 0.F);
        Ensures(current_s >= 0.F);
    }

    void copy_best_state(span<const int16> source_state) const
    {
        Expects(source_state.size() == best_state.size());
        Expects(source_state.size() == current_state.size());
        Expects(!source_state.empty());

        ranges::copy(source_state, best_state.begin());
        ranges::copy(source_state, current_state.begin());
    }

    void init_states(index state_size)
    {
        Expects(state_size > 0);

        int_data.resize(static_cast<gsl::index>(state_size) * 3);
        auto* data = int_data.data();
        const auto next = [&] {
            return std::exchange(data, std::next(data, state_size));
        };
        best_state = {next(), state_size};
        current_state = {next(), state_size};
        target_state = {next(), state_size};

        Ensures(!current_state.empty());
        Ensures(!target_state.empty());
        Ensures(!best_state.empty());
    }

    void init_energies()
    {
        Expects(!current_state.empty());

        current_e = (*makespan)(current_state);
        lowest_e = current_e;
        current_s = stun(lowest_e, current_e, gamma);

        Ensures(current_e >= 0.F);
        Ensures(lowest_e >= 0.F);
        Ensures(current_s >= 0.F && current_s <= 1.F);
    }

    void run() noexcept
    {
#ifdef UNIT_TEST
        constexpr auto max_iterations = 2;
#endif // UNIT_TEST
        for (current_iteration = 0;
             current_iteration < max_iterations;
             ++current_iteration) {
            get_new_neighbor();
            if (neighbor_is_better()) continue;
            perform_stun();
        }
    }
};

} // namespace

namespace angonoka::stun {

STUNResult stochastic_tunneling(
    RandomUtilsT& random_utils,
    MakespanEstimatorT& makespan,
    span<const int16> state,
    Gamma gamma,
    Beta beta,
    BetaScale beta_scale)
{
    StochasticTunneling stun_op{
        .random_utils{&random_utils},
        .makespan{&makespan},
        .beta_driver{beta, beta_scale},
        .gamma{gamma}};

    stun_op.init_states(state.size());
    stun_op.copy_best_state(state);
    stun_op.init_energies();
    stun_op.run();

    stun_op.int_data.resize(static_cast<gsl::index>(state.size()));

    return {
        stun_op.lowest_e,
        std::move(stun_op.int_data),
        stun_op.beta_driver.beta()};
}

} // namespace angonoka::stun
