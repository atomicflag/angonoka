#include "simulation.h"

namespace angonoka {
namespace detail {
    Simulation::Simulation(
            const Configuration& config,
            const OptimizedSchedule& schedule,
            stun::RandomUtils& random) : config{&config}, schedule{&schedule}, random{&random} {

    }

        [[nodiscard]] std::chrono::seconds Simulation::operator()() noexcept {
            return {};
        }
}
}
