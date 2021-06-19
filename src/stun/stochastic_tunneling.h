#pragma once

#include "detail.h"
#include "schedule.h"
#include <gsl/gsl-lite.hpp>
#include <vector>
#ifndef UNIT_TEST
#include "random_utils.h"
#include "temperature.h"
#include "utils.h"
#else // UNIT_TEST
#include "stub/random_utils.h"
#include "stub/temperature.h"
#include "stub/utils.h"
#endif // UNIT_TEST

namespace angonoka::stun {

struct ScheduleParams;
/**
    Stochastic tunneling algorithm.

    The internal schedule can be updated as many times as
    needed.
*/
class StochasticTunneling {
public:
    /**
        Result of a stochastic tunneling pass.

        @var schedule       Schedule that had the lowest energy
        @var energy         Lowest energy achieved so far
        @var temperature    Final temperature
    */
    struct Result {
        std::vector<ScheduleItem> schedule;
        float energy;
        float temperature;
    };

    /**
        STUN auxiliary data and utilities.

        @var mutator    Instance of Mutator
        @var random     Instance of RandomUtils
        @var makespan   Instance of Makespan
        @var temp       Instance of Temperature
        @var gamma      Tunneling parameter
    */
    struct Options {
        gsl::not_null<const Mutator*> mutator;
        gsl::not_null<RandomUtils*> random;
        gsl::not_null<Makespan*> makespan;
        gsl::not_null<Temperature*> temp;
        float gamma;
    };

    /**
        Default constructor.

        The object will be in an uninitialized state. User must call
        reset to set the initial schedule.

        @param options Instance of StochasticTunneling::Options
    */
    StochasticTunneling(const Options& options);

    /**
        Constructor.

        @param options  Instance of StochasticTunneling::Options
        @param schedule Initial schedule
    */
    StochasticTunneling(const Options& options, Schedule schedule);

    StochasticTunneling(const StochasticTunneling& other);
    StochasticTunneling(StochasticTunneling&& other) noexcept;
    StochasticTunneling& operator=(const StochasticTunneling& other);
    StochasticTunneling&
    operator=(StochasticTunneling&& other) noexcept;
    ~StochasticTunneling() noexcept;

    /**
        Reset stochastic tunneling algorithm to a new
        schedule.

        @param schedule Initial schedule
    */
    void reset(Schedule schedule);

    /**
        Set stochastic tunneling options.

        @param options Instance of Options
    */
    void options(const Options& options);

    /**
        Get stochastic tunneling options.

        @return Stochastic tunneling options.
    */
    [[nodiscard]] Options options() const;

    /**
        Update the internal state according to stochastic
        tunneling algorithm.
    */
    void update() noexcept;

    /**
        The best schedule so far.

        @return A schedule.
    */
    [[nodiscard]] Schedule schedule() const;

    /**
        The best makespan so far.

        @return Makespan.
    */
    [[nodiscard]] float normalized_makespan() const;

private:
    struct Impl;

    using index = MutSchedule::index_type;

    gsl::not_null<const Mutator*> mutator;
    gsl::not_null<RandomUtils*> random;
    gsl::not_null<Makespan*> makespan;
    gsl::not_null<Temperature*> temp;

    std::vector<ScheduleItem> schedule_buffer;
    MutSchedule best_schedule;
    MutSchedule current_schedule;
    MutSchedule target_schedule;

    float current_e;
    float lowest_e;
    float target_e;

    float gamma;
    float current_s;
    float target_s;
};

} // namespace angonoka::stun
