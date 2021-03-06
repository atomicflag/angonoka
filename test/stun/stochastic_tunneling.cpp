#include "stun/stochastic_tunneling.h"
#include "stun/random_utils.h"
#include "stun/schedule_info.h"
#include "stun/temperature.h"
#include "stun/utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

namespace {
using namespace angonoka::stun;

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
    MAKE_MOCK1(update, void(float stun), noexcept override);
    MAKE_MOCK0(average_stun, float(), const noexcept override);
};
} // namespace

TEST_CASE("StochasticTunneling type traits")
{
    using angonoka::stun::StochasticTunneling;
    STATIC_REQUIRE(
        std::is_nothrow_destructible_v<StochasticTunneling>);
    STATIC_REQUIRE(
        !std::is_default_constructible_v<StochasticTunneling>);
    STATIC_REQUIRE(std::is_copy_constructible_v<StochasticTunneling>);
    STATIC_REQUIRE(std::is_copy_assignable_v<StochasticTunneling>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<StochasticTunneling>);
    STATIC_REQUIRE(
        std::is_nothrow_move_assignable_v<StochasticTunneling>);
}

TEST_CASE("Stochastic tunneling")
{
    using namespace angonoka::stun;
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
    REQUIRE_CALL(temperature, update(_)).IN_SEQUENCE(seq);

    REQUIRE_CALL(mutator, call(_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(makespan, call(_)).RETURN(.1F).IN_SEQUENCE(seq);

    SECTION("Simple")
    {
        StochasticTunneling stun{
            {.mutator{&mutator},
             .random{&random_utils},
             .makespan{&makespan},
             .temp{&temperature},
             .gamma{.5F}},
            state};
        for (int i{0}; i < 2; ++i) stun.update();

        REQUIRE(stun.energy() == Approx(.1F));
        REQUIRE(stun.state().size() == 3);
    }

    SECTION("Two phase construction")
    {
        StochasticTunneling stun{
            {.mutator{&mutator},
             .random{&random_utils},
             .makespan{&makespan},
             .temp{&temperature},
             .gamma{.5F}}};
        stun.reset(state);
        for (int i{0}; i < 2; ++i) stun.update();

        REQUIRE(stun.energy() == Approx(.1F));
        REQUIRE(stun.state().size() == 3);
    }
}

TEST_CASE("StochasticTunneling special member functions")
{
    using namespace angonoka::stun;
    using trompeloeil::_;

    RandomUtilsMock random_utils;
    MakespanMock makespan;
    TemperatureMock temperature;
    MutatorMock mutator;
    std::vector<StateItem> state{{0, 0}, {1, 1}, {2, 2}};
    std::vector<StateItem> state2{{3, 3}, {4, 4}, {5, 5}};

    ALLOW_CALL(makespan, call(_)).RETURN(1.F);

    StochasticTunneling::Options options{
        .mutator{&mutator},
        .random{&random_utils},
        .makespan{&makespan},
        .temp{&temperature},
        .gamma{.5F}};

    StochasticTunneling stun{options, state};

    SECTION("Copy assignment")
    {
        StochasticTunneling stun2{options, state2};
        stun = stun2;

        REQUIRE(stun.state()[0].agent_id == 3);
    }

    SECTION("Move assignment")
    {
        StochasticTunneling stun2{options, state2};
        stun = std::move(stun2);

        REQUIRE(stun.state()[0].agent_id == 3);
    }

    SECTION("Copy ctor")
    {
        StochasticTunneling stun2{stun};

        REQUIRE(stun2.state()[1].agent_id == 1);
    }

    SECTION("Move ctor")
    {
        StochasticTunneling stun2{std::move(stun)};

        REQUIRE(stun2.state()[1].agent_id == 1);
    }

    SECTION("Self copy")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
        stun = stun;
#pragma clang diagnostic pop
        REQUIRE(stun.state()[1].agent_id == 1);
    }

    SECTION("Self move")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
        stun = std::move(stun);
#pragma clang diagnostic pop
        REQUIRE(stun.state()[1].agent_id == 1);
    }
}
