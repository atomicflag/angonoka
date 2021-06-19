#include "stun/stochastic_tunneling.h"
#include "stub/random_utils.h"
#include "stub/temperature.h"
#include "stub/utils.h"
#include "stun/schedule_params.h"
#include <boost/ut.hpp>
#include <trompeloeil.hpp>

using namespace boost::ut;

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

suite stochastic_tunneling = [] {
    "StochasticTunneling type traits"_test = [] {
        using angonoka::stun::StochasticTunneling;
        expect(std::is_nothrow_destructible_v<StochasticTunneling>);
        expect(!std::is_default_constructible_v<StochasticTunneling>);
        expect(std::is_copy_constructible_v<StochasticTunneling>);
        expect(std::is_copy_assignable_v<StochasticTunneling>);
        expect(std::is_nothrow_move_constructible_v<
               StochasticTunneling>);
        expect(
            std::is_nothrow_move_assignable_v<StochasticTunneling>);
    };

    constexpr auto run_mock = [](auto test_fn) {
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

        test_fn(
            mutator,
            random_utils,
            makespan,
            temperature,
            schedule);
    };

    "stochastic tunneling"_test = [&] {
        using namespace angonoka::stun;
        "simple"_test = [&] {
            run_mock([](auto& mutator,
                        auto& random_utils,
                        auto& makespan,
                        auto& temperature,
                        auto& schedule) {
                StochasticTunneling stun{
                    {.mutator{&mutator},
                     .random{&random_utils},
                     .makespan{&makespan},
                     .temp{&temperature},
                     .gamma{.5F}},
                    schedule};
                for (int i{0}; i < 2; ++i) stun.update();

                expect(stun.normalized_makespan() == .1_d);
                expect(stun.schedule().size() == 3);
            });
        };

        "two phase construction"_test = [&] {
            run_mock([](auto& mutator,
                        auto& random_utils,
                        auto& makespan,
                        auto& temperature,
                        auto& schedule) {
                StochasticTunneling stun{
                    {.mutator{&mutator},
                     .random{&random_utils},
                     .makespan{&makespan},
                     .temp{&temperature},
                     .gamma{.5F}}};
                stun.reset(schedule);
                for (int i{0}; i < 2; ++i) stun.update();

                expect(stun.normalized_makespan() == .1_d);
                expect(stun.schedule().size() == 3);
            });
        };
    };

    "StochasticTunneling options"_test = [] {
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

        expect(stun.options().makespan == &makespan);

        MakespanMock makespan2;

        options.makespan = &makespan2;
        stun.options(options);

        expect(stun.options().makespan == &makespan2);
    };

    "StochasticTunneling special member functions"_test = [] {
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

        "copy assignment"_test = [&] {
            StochasticTunneling stun{options, schedule};
            StochasticTunneling stun2{options, schedule2};
            stun = stun2;

            expect(stun.schedule()[0].agent_id == 3);
        };

        "move assignment"_test = [&] {
            StochasticTunneling stun{options, schedule};
            StochasticTunneling stun2{options, schedule2};
            stun = std::move(stun2);

            expect(stun.schedule()[0].agent_id == 3);
        };

        "copy ctor"_test = [&] {
            StochasticTunneling stun{options, schedule};
            StochasticTunneling stun2{stun};

            expect(stun2.schedule()[1].agent_id == 1);
        };

        "move ctor"_test = [&] {
            StochasticTunneling stun{options, schedule};
            StochasticTunneling stun2{std::move(stun)};

            expect(stun2.schedule()[1].agent_id == 1);
        };

        "self copy"_test = [&] {
            StochasticTunneling stun{options, schedule};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            stun = stun;
#pragma clang diagnostic pop
            expect(stun.schedule()[1].agent_id == 1);
        };

        "self move"_test = [&] {
            StochasticTunneling stun{options, schedule};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            stun = std::move(stun);
#pragma clang diagnostic pop
            expect(stun.schedule()[1].agent_id == 1);
        };
    };
};
