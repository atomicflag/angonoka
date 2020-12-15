#include "stun_dag/stochastic_tunneling.h"
#include "stun_dag/random_utils.h"
#include "stun_dag/schedule_info.h"
#include "stun_dag/temperature.h"
#include "stun_dag/utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace {
using namespace angonoka::stun_dag;

struct RandomUtilsMock final : RandomUtilsStub {
    MAKE_MOCK0(uniform_01, float(), noexcept override);
    MAKE_MOCK1(
        uniform_int,
        int16(std::int16_t max),
        noexcept override);
};

struct MakespanMock final : MakespanStub {
    float operator()(State state) noexcept override
    {
        return run(state);
    }
    MAKE_MOCK1(run, float(State state), noexcept);
};

struct TemperatureMock final : TemperatureStub {
    operator float() noexcept override { return to_float(); }
    MAKE_MOCK0(to_float, float(), noexcept);
    MAKE_MOCK2(
        update,
        void(float stun, float dampening),
        noexcept override);
    MAKE_MOCK0(average_stun, float(), const noexcept override);
};
} // namespace

TEST_CASE("Stochastic tunneling")
{
    using namespace angonoka::stun_dag;

    RandomUtilsMock random_utils;
    MakespanMock makespan;
    TemperatureMock temperature;
    ScheduleInfo info;
    info.task_duration.resize(3); // TEMP
    std::vector<StateItem> state(3);
    // TODO: WIP
    const auto r = stochastic_tunneling(
        state,
        STUNOptions{
            .info{&info},
            .random{&random_utils},
            .makespan{&makespan},
            .temp{&temperature}});
}
