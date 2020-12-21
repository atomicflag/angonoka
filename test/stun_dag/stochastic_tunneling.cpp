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
        return call(state);
    }
    MAKE_MOCK1(call, float(State state), noexcept);
};

struct MutatorMock final : MutatorStub {
    void operator()(MutState state) const noexcept override
    {
        return call(state);
    }
    MAKE_MOCK1(call, void(MutState state), const noexcept);
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
    using trompeloeil::_;

    RandomUtilsMock random_utils;
    MakespanMock makespan;
    TemperatureMock temperature;
    MutatorMock mutator;
    std::vector<StateItem> state(3);

    trompeloeil::sequence seq;

    REQUIRE_CALL(makespan, call(state)).RETURN(1.F).IN_SEQUENCE(seq);
    REQUIRE_CALL(mutator, call(_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(makespan, call(_)).RETURN(1.F).IN_SEQUENCE(seq);
    REQUIRE_CALL(temperature, to_float())
        .RETURN(.5F)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(random_utils, uniform_01())
        .RETURN(.1F)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(temperature, update(_, _)).IN_SEQUENCE(seq);

    REQUIRE_CALL(mutator, call(_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(makespan, call(_)).RETURN(.1F).IN_SEQUENCE(seq);
    REQUIRE_CALL(temperature, to_float())
        .RETURN(.5F)
        .IN_SEQUENCE(seq);

    const auto r = stochastic_tunneling(
        state,
        STUNOptions{
            .mutator{&mutator},
            .random{&random_utils},
            .makespan{&makespan},
            .temp{&temperature}});

    REQUIRE(r.energy == Approx(.1F));
    REQUIRE(r.temperature == Approx(.5F));
}
