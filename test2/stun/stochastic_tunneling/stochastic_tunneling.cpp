#include "stun/stochastic_tunneling.h"
#include "stub/random_utils.h"
#include "stub/temperature.h"
#include "stub/utils.h"
#include "stun/schedule_params.h"
#include <catch2/catch.hpp>
#include <trompeloeil.hpp>

namespace {
using namespace angonoka::stun;

struct RandomUtilsMock final : RandomUtils {
    MAKE_MOCK0(uniform_01, float(), noexcept override);
    MAKE_MOCK1(
        uniform_int,
        int16(std::int16_t max),
        noexcept override);
};

struct MakespanMock final : Makespan {
    float operator()(Schedule schedule) noexcept override
    {
        return call(schedule);
    }
    MAKE_MOCK1(call, float(Schedule schedule), noexcept);
};

struct MutatorMock final : Mutator {
    void operator()(MutSchedule schedule) const noexcept override
    {
        return call(schedule);
    }
    MAKE_MOCK1(call, void(MutSchedule schedule), const noexcept);
};

struct TemperatureMock final : Temperature {
    operator float() noexcept override { return to_float(); }
    MAKE_MOCK0(to_float, float(), noexcept);
    MAKE_MOCK1(update, void(float stun), noexcept override);
    MAKE_MOCK0(average_stun, float(), const noexcept override);
};
} // namespace

TEST_CASE("StochasticTunneling ")
{
    SECTION("StochasticTunneling type traits")
    {
        using angonoka::stun::StochasticTunneling;
        STATIC_REQUIRE(
            std::is_nothrow_destructible_v<StochasticTunneling>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<StochasticTunneling>);
        STATIC_REQUIRE(
            std::is_copy_constructible_v<StochasticTunneling>);
        STATIC_REQUIRE(
            std::is_copy_assignable_v<StochasticTunneling>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<
                       StochasticTunneling>);
        STATIC_REQUIRE(
            std::is_nothrow_move_assignable_v<StochasticTunneling>);
    }

    SECTION("stochastic tunneling")
    {
        using namespace angonoka::stun;
        using trompeloeil::_;

        RandomUtilsMock random_utils;
        MakespanMock makespan;
        TemperatureMock temperature;
        MutatorMock mutator;
        std::vector<ScheduleItem> schedule(3);

        trompeloeil::sequence seq;

        REQUIRE_CALL(makespan, call(schedule))
            .RETURN(1.F)
            .IN_SEQUENCE(seq);
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

        SECTION("simple")
        {
            StochasticTunneling stun{
                {.mutator{&mutator},
                 .random{&random_utils},
                 .makespan{&makespan},
                 .temp{&temperature},
                 .gamma{.5F}},
                schedule};
            for (int i{0}; i < 2; ++i) stun.update();

            REQUIRE(stun.normalized_makespan() == Approx(.1));
            REQUIRE(stun.schedule().size() == 3);
        }

        SECTION("two phase construction")
        {
            StochasticTunneling stun{
                {.mutator{&mutator},
                 .random{&random_utils},
                 .makespan{&makespan},
                 .temp{&temperature},
                 .gamma{.5F}}};
            stun.reset(schedule);
            for (int i{0}; i < 2; ++i) stun.update();

            REQUIRE(stun.normalized_makespan() == Approx(.1));
            REQUIRE(stun.schedule().size() == 3);
        }
    }

    SECTION("StochasticTunneling options")
    {
        using namespace angonoka::stun;
        using trompeloeil::_;

        RandomUtilsMock random_utils;
        MakespanMock makespan;
        TemperatureMock temperature;
        MutatorMock mutator;
        std::vector<ScheduleItem> schedule{{0, 0}, {1, 1}, {2, 2}};

        ALLOW_CALL(makespan, call(_)).RETURN(1.F);

        StochasticTunneling::Options options{
            .mutator{&mutator},
            .random{&random_utils},
            .makespan{&makespan},
            .temp{&temperature},
            .gamma{.5F}};

        StochasticTunneling stun{options, schedule};

        REQUIRE(stun.options().makespan == &makespan);

        MakespanMock makespan2;

        options.makespan = &makespan2;
        stun.options(options);

        REQUIRE(stun.options().makespan == &makespan2);
    }

    SECTION("StochasticTunneling special member functions")
    {
        using namespace angonoka::stun;
        using trompeloeil::_;

        RandomUtilsMock random_utils;
        MakespanMock makespan;
        TemperatureMock temperature;
        MutatorMock mutator;
        std::vector<ScheduleItem> schedule{{0, 0}, {1, 1}, {2, 2}};
        std::vector<ScheduleItem> schedule2{{3, 3}, {4, 4}, {5, 5}};

        ALLOW_CALL(makespan, call(_)).RETURN(1.F);

        StochasticTunneling::Options options{
            .mutator{&mutator},
            .random{&random_utils},
            .makespan{&makespan},
            .temp{&temperature},
            .gamma{.5F}};

        StochasticTunneling stun{options, schedule};

        SECTION("copy assignment")
        {
            StochasticTunneling stun2{options, schedule2};
            stun = stun2;

            REQUIRE(stun.schedule()[0].agent_id == 3);
        }

        SECTION("move assignment")
        {
            StochasticTunneling stun2{options, schedule2};
            stun = std::move(stun2);

            REQUIRE(stun.schedule()[0].agent_id == 3);
        }

        SECTION("copy ctor")
        {
            StochasticTunneling stun2{stun};

            REQUIRE(stun2.schedule()[1].agent_id == 1);
        }

        SECTION("move ctor")
        {
            StochasticTunneling stun2{std::move(stun)};

            REQUIRE(stun2.schedule()[1].agent_id == 1);
        }

        SECTION("self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            stun = stun;
#pragma clang diagnostic pop
            REQUIRE(stun.schedule()[1].agent_id == 1);
        }

        SECTION("self move")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            stun = std::move(stun);
#pragma clang diagnostic pop
            REQUIRE(stun.schedule()[1].agent_id == 1);
        }
    }
}
