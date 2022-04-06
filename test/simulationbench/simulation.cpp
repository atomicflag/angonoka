#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include "config/load.h"
#include "predict.h"
#include "simulation.h"

int main()
{
    using namespace angonoka;

    const auto config = load_file("tasks.yml");
    auto [schedule_future, event_queue] = schedule(config);
    const auto schedule = schedule_future.get();

    stun::RandomUtils random;

    detail::Simulation sim{{.config{&config}, .random{&random}}};

    ankerl::nanobench::Bench().run("simulation", [&] {
        const auto duration = sim(schedule.schedule);
        ankerl::nanobench::doNotOptimizeAway(duration);
    });
}
