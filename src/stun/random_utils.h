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
    Various randomization routines.
*/
class RandomUtils {
public:
    /**
        Constructor.

        @param task_agents Agent ids for each task
    */
    RandomUtils(gsl::not_null<const TaskAgents*> task_agents);

    /**
        Randomly permutes a single task in the state.

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
} // namespace angonoka::stun
