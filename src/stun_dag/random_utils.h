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
        TODO: Doc
        Default constructor.
    */
    RandomUtils();

    /**
        TODO: Update doc
        Constructor with the PRNG seed.

        @param seed         Random engine seed
    */
    RandomUtils(gsl::index seed);

    /**
        Uniformally distributed real value between 0 and 1.

        @return Random number
    */
    float get_uniform() noexcept;

    /**
        TODO: Doc
    */
    int get_uniform_int(int16 max) noexcept;

private:
    RandomEngine generator{
        pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> uniform;
    boost::random::uniform_int_distribution<> uniform_int;
};
} // namespace angonoka::stun_dag
