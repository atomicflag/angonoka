#pragma once

#include "common.h"
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <gsl/gsl-lite.hpp>
#include <pcg_random.hpp>
#include <random>

namespace angonoka::stun_dag {
using RandomEngine = pcg32;

/**
    Group of functions that use the PRNG.
*/
class RandomUtils {
public:
    /**
        TODO: Doc, implement
        Default constructor.
    */
    RandomUtils(int16 state_size);

    /**
        TODO: Update doc, implement
        Constructor with the PRNG seed.

        @param seed         Random engine seed
    */
    RandomUtils(int16 state_size, gsl::index seed);

    /**
        Uniformally distributed real value between 0 and 1.

        @return Random number
    */
    float get_uniform() noexcept;

    /**
        TODO: Doc, implement
    */
    int get_random_index() noexcept;

private:
    RandomEngine generator{
        pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> uniform;
    boost::random::uniform_int_distribution<> uniform_int;
};
} // namespace angonoka::stun_dag
