#include "stochastic_tunneling.h"
#include "random_utils.h"
#include "schedule_params.h"
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
/**
    Implementation details.
*/
struct StochasticTunneling::Impl {
    /**
        Creates a new (mutated) state from the current state.
    */
    static void get_new_neighbor(StochasticTunneling& self) noexcept
    {
        Expects(!self.current_state.empty());
        Expects(!self.target_state.empty());

        ranges::copy(self.current_state, self.target_state.begin());
        (*self.mutator)(self.target_state);
        self.target_e = (*self.makespan)(self.target_state);

        Ensures(self.target_e >= 0.F);
    }

    /**
        Updates the lowest energy and best state if the
        target state is better.
    */
    static bool neighbor_is_better(StochasticTunneling& self) noexcept
    {
        Expects(self.target_e >= 0.F);
        Expects(self.current_e >= 0.F);
        Expects(self.lowest_e >= 0.F);
        Expects(self.current_e >= self.lowest_e);
        Expects(!self.target_state.empty());
        Expects(!self.current_state.empty());
        Expects(!self.best_state.empty());

        if (self.target_e < self.current_e) {
            if (self.target_e < self.lowest_e) {
                self.lowest_e = self.target_e;
                ranges::copy(
                    self.target_state,
                    self.best_state.begin());
                self.current_s = stun_fn(
                    self.lowest_e,
                    self.current_e,
                    self.gamma);
            }
            std::swap(self.current_state, self.target_state);
            self.current_e = self.target_e;
            return true;
        }
        return false;
    }

    /**
        Perform Monte Carlo sampling on the STUN-adjusted energy.
    */
    static void perform_stun(StochasticTunneling& self) noexcept
    {
        Expects(self.target_e >= 0.F);
        Expects(self.lowest_e >= 0.F);
        Expects(self.current_s >= 0.F);
        Expects(!self.target_state.empty());
        Expects(!self.current_state.empty());

        self.target_s
            = stun_fn(self.lowest_e, self.target_e, self.gamma);
        const auto delta_s = self.target_s - self.current_s;
        const auto pr
            = std::min(1.F, std::exp(-*self.temp * delta_s));
        if (pr >= self.random->uniform_01()) {
            std::swap(self.current_state, self.target_state);
            self.current_e = self.target_e;
            self.current_s = self.target_s;
            self.temp->update(self.target_s);
        }

        Ensures(self.target_s >= 0.F);
        Ensures(self.current_s >= 0.F);
    }

    /**
        Init energies and STUN-adjusted energies.
    */
    static void init_energies(StochasticTunneling& self)
    {
        Expects(!self.current_state.empty());

        self.current_e = (*self.makespan)(self.current_state);
        self.lowest_e = self.current_e;
        self.current_s
            = stun_fn(self.lowest_e, self.current_e, self.gamma);

        Ensures(self.current_e >= 0.F);
        Ensures(self.lowest_e >= 0.F);
        Ensures(self.current_s >= 0.F && self.current_s <= 1.F);
    }

    /**
        Recreate state spans over the state buffer object.

        @param state_size Size of the state
    */
    static void
    prepare_state_spans(StochasticTunneling& self, index state_size)
    {
        Expects(state_size > 0);

        self.state_buffer.resize(
            static_cast<gsl::index>(state_size) * 3);
        auto* data = self.state_buffer.data();
        const auto next = [&] {
            return std::exchange(data, std::next(data, state_size));
        };
        self.best_state = {next(), state_size};
        self.current_state = {next(), state_size};
        self.target_state = {next(), state_size};

        Ensures(self.current_state.size() == state_size);
        Ensures(self.target_state.size() == state_size);
        Ensures(self.best_state.size() == state_size);
    }

    /**
        Init all states with the source state.

        @param source_state Source state
    */
    static void
    init_states(StochasticTunneling& self, State source_state)
    {
        Expects(source_state.size() == self.best_state.size());
        Expects(source_state.size() == self.current_state.size());
        Expects(!source_state.empty());

        ranges::copy(source_state, self.best_state.begin());
        ranges::copy(source_state, self.current_state.begin());
    }
};

void StochasticTunneling::update() noexcept
{
    Expects(!state_buffer.empty());
    Expects(!current_state.empty());
    Impl::get_new_neighbor(*this);
    if (Impl::neighbor_is_better(*this)) return;
    Impl::perform_stun(*this);
}

void StochasticTunneling::reset(State state)
{
    Expects(!state.empty());
    Impl::prepare_state_spans(*this, state.size());
    Impl::init_states(*this, state);
    Impl::init_energies(*this);
}

State StochasticTunneling::state() const
{
    Expects(!state_buffer.empty());
    Expects(!best_state.empty());
    return best_state;
}

float StochasticTunneling::energy() const
{
    Expects(!state_buffer.empty());
    return lowest_e;
}

StochasticTunneling::StochasticTunneling(const Options& options)
    : mutator{options.mutator}
    , random{options.random}
    , makespan{options.makespan}
    , temp{options.temp}
    , gamma{options.gamma}
{
}

StochasticTunneling::StochasticTunneling(
    const Options& options,
    State state)
    : StochasticTunneling{options}
{
    Expects(!state.empty());
    reset(state);
}

StochasticTunneling::StochasticTunneling(
    const StochasticTunneling& other)
    : mutator{other.mutator}
    , random{other.random}
    , makespan{other.makespan}
    , temp{other.temp}
    , state_buffer{other.state_buffer}
    , current_e{other.current_e}
    , lowest_e{other.lowest_e}
    , target_e{other.target_e}
    , gamma{other.gamma}
    , current_s{other.current_s}
    , target_s{other.target_s}
{
    if (!other.best_state.empty())
        Impl::prepare_state_spans(*this, other.best_state.size());
}

StochasticTunneling&
StochasticTunneling::operator=(const StochasticTunneling& other)
{
    *this = StochasticTunneling{other};
    return *this;
}

StochasticTunneling::~StochasticTunneling() noexcept = default;
StochasticTunneling::StochasticTunneling(
    StochasticTunneling&& other) noexcept = default;
StochasticTunneling&
StochasticTunneling::operator=(StochasticTunneling&& other) noexcept
{
    if (&other == this) return *this;

    mutator = std::move(other.mutator);
    random = std::move(other.random);
    makespan = std::move(other.makespan);
    temp = std::move(other.temp);
    state_buffer = std::move(other.state_buffer);
    best_state = other.best_state;
    current_state = other.current_state;
    target_state = other.target_state;
    current_e = other.current_e;
    lowest_e = other.lowest_e;
    target_e = other.target_e;
    gamma = other.gamma;
    current_s = other.current_s;
    target_s = other.target_s;

    return *this;
}

void StochasticTunneling::options(const Options& options)
{
    mutator = options.mutator;
    random = options.random;
    makespan = options.makespan;
    temp = options.temp;
    gamma = options.gamma;
}

StochasticTunneling::Options StochasticTunneling::options() const
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbraced-scalar-init"
    return {
        .mutator{mutator},
        .random{random},
        .makespan{makespan},
        .temp{temp},
        .gamma{gamma}};
#pragma clang diagnostic pop
}
} // namespace angonoka::stun
