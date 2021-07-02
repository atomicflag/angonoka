#include "stochastic_tunneling.h"
#include "schedule_params.h"
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
        Creates a new (mutated) schedule from the current
        schedule.
    */
    static void get_new_neighbor(StochasticTunneling& self) noexcept
    {
        Expects(!self.current_schedule.empty());
        Expects(!self.target_schedule.empty());

        ranges::copy(
            self.current_schedule,
            self.target_schedule.begin());
        (*self.mutator)(self.target_schedule);
        self.target_e = (*self.makespan)(self.target_schedule);

        Ensures(self.target_e >= 0.F);
    }

    /**
        Updates the lowest energy and best schedule if the
        target schedule is better.

        @return True if target schedule is better than the current
       schedule.
    */
    static bool neighbor_is_better(StochasticTunneling& self) noexcept
    {
        Expects(self.target_e >= 0.F);
        Expects(self.current_e >= 0.F);
        Expects(self.lowest_e >= 0.F);
        Expects(self.current_e >= self.lowest_e);
        Expects(!self.target_schedule.empty());
        Expects(!self.current_schedule.empty());
        Expects(!self.best_schedule.empty());

        if (self.target_e < self.current_e) {
            if (self.target_e < self.lowest_e) {
                self.lowest_e = self.target_e;
                ranges::copy(
                    self.target_schedule,
                    self.best_schedule.begin());
                self.current_s = stun_fn(
                    self.lowest_e,
                    self.current_e,
                    self.gamma);
            }
            std::swap(self.current_schedule, self.target_schedule);
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
        Expects(!self.target_schedule.empty());
        Expects(!self.current_schedule.empty());

        self.target_s
            = stun_fn(self.lowest_e, self.target_e, self.gamma);
        const auto delta_s = self.target_s - self.current_s;
        const auto pr
            = std::min(1.F, std::exp(-*self.temp * delta_s));
        if (pr >= self.random->uniform_01()) {
            std::swap(self.current_schedule, self.target_schedule);
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
        Expects(!self.current_schedule.empty());

        self.current_e = (*self.makespan)(self.current_schedule);
        self.lowest_e = self.current_e;
        self.current_s
            = stun_fn(self.lowest_e, self.current_e, self.gamma);

        Ensures(self.current_e >= 0.F);
        Ensures(self.lowest_e >= 0.F);
        Ensures(self.current_s >= 0.F && self.current_s <= 1.F);
    }

    /**
        Recreate schedule spans over the schedule buffer object.

        @param schedule_size Size of the schedule
    */
    static void
    prepare_schedules(StochasticTunneling& self, index schedule_size)
    {
        Expects(schedule_size > 0);

        self.schedule_buffer.resize(
            static_cast<gsl::index>(schedule_size) * 3);
        auto* data = self.schedule_buffer.data();
        const auto next = [&] {
            return std::exchange(
                data,
                std::next(data, schedule_size));
        };
        self.best_schedule = {next(), schedule_size};
        self.current_schedule = {next(), schedule_size};
        self.target_schedule = {next(), schedule_size};

        Ensures(self.current_schedule.size() == schedule_size);
        Ensures(self.target_schedule.size() == schedule_size);
        Ensures(self.best_schedule.size() == schedule_size);
    }

    /**
        Init all schedules with the source schedule.

        @param source_schedule Initial schedule
    */
    static void init_schedules(
        StochasticTunneling& self,
        Schedule source_schedule)
    {
        Expects(source_schedule.size() == self.best_schedule.size());
        Expects(
            source_schedule.size() == self.current_schedule.size());
        Expects(!source_schedule.empty());

        ranges::copy(source_schedule, self.best_schedule.begin());
        ranges::copy(source_schedule, self.current_schedule.begin());
    }
};

void StochasticTunneling::update() noexcept
{
    Expects(!schedule_buffer.empty());
    Expects(!current_schedule.empty());
    Impl::get_new_neighbor(*this);
    if (Impl::neighbor_is_better(*this)) return;
    Impl::perform_stun(*this);
}

void StochasticTunneling::reset(Schedule schedule)
{
    Expects(!schedule.empty());
    Impl::prepare_schedules(*this, schedule.size());
    Impl::init_schedules(*this, schedule);
    Impl::init_energies(*this);
}

Schedule StochasticTunneling::schedule() const
{
    Expects(!schedule_buffer.empty());
    Expects(!best_schedule.empty());
    return best_schedule;
}

float StochasticTunneling::normalized_makespan() const
{
    Expects(!schedule_buffer.empty());
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
    Schedule schedule)
    : StochasticTunneling{options}
{
    Expects(!schedule.empty());
    reset(schedule);
}

StochasticTunneling::StochasticTunneling(
    const StochasticTunneling& other)
    : mutator{other.mutator}
    , random{other.random}
    , makespan{other.makespan}
    , temp{other.temp}
    , schedule_buffer{other.schedule_buffer}
    , current_e{other.current_e}
    , lowest_e{other.lowest_e}
    , target_e{other.target_e}
    , gamma{other.gamma}
    , current_s{other.current_s}
    , target_s{other.target_s}
{
    if (!other.best_schedule.empty())
        Impl::prepare_schedules(*this, other.best_schedule.size());
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
    schedule_buffer = std::move(other.schedule_buffer);
    best_schedule = other.best_schedule;
    current_schedule = other.current_schedule;
    target_schedule = other.target_schedule;
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
