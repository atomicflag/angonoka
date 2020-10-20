#pragma once

#include "common.h"
#include <boost/random/uniform_01.hpp>
#include <gsl/gsl-lite.hpp>
#include <pcg_random.hpp>
#include <random>
#include <range/v3/view/span.hpp>

namespace angonoka::stun {
using ranges::span;
using RandomEngine = pcg32;

class TaskAgents;

/**
    Assortment of functions that use the PRNG.
*/
class RandomUtils {
public:
    /**
        Constructor.

        @param task_agents Agent ids for each task
    */
    RandomUtils(gsl::not_null<const TaskAgents*> task_agents);

    /**
        Constructor with the PRNG seed for debug.

        @param task_agents  Agent ids for each task
        @param seed         Random engine seed
    */
    RandomUtils(
        gsl::not_null<const TaskAgents*> task_agents,
        gsl::index seed);

    /**
        Assigns a new agent to a random task.

        @param state An array of agent ids for each task
    */
    void get_neighbor_inplace(span<int16> state) noexcept;

    /**
        Uniformally distributed real value between 0 and 1.

        @return Random number
    */
    float get_uniform() noexcept;

private:
    gsl::not_null<const TaskAgents*> task_agents;
    RandomEngine generator{
        pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> uniform;

    /**
        Pick a random index from 0 to max-1.

        @param max The size of the array

        @return A random index within the array
    */
    index random_index(index max) noexcept;

    /**
        Retrieve a random element of the array.

        @param range The target range

        @return A randomly picked element of the array
    */
    int16 pick_random(span<const int16> range) noexcept;
};

#ifdef UNIT_TEST
struct RandomUtilsStub {
    virtual void get_neighbor_inplace(span<int16> state) noexcept = 0;
    virtual float get_uniform() noexcept = 0;
    virtual ~RandomUtilsStub() noexcept = default;
};
#endif // UNIT_TEST
} // namespace angonoka::stun
