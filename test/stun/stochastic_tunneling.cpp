#include "stun/stochastic_tunneling.h"
#include "stun/makespan_estimator.h"
#include "stun/random_utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <vector>

namespace {
using namespace angonoka::stun;
struct RandomUtilsMock final : RandomUtilsStub {
    MAKE_MOCK1(
        get_neighbor_inplace,
        void(span<int16>),
        noexcept override);
    MAKE_MOCK0(get_uniform, float(), noexcept override);
};

struct MakespanEstimatorMock final : MakespanEstimatorStub {
    float operator()(span<const int16> state) noexcept override
    {
        return call(state);
    }
    MAKE_MOCK1(call, float(span<const int16>));
};
} // namespace

TEST_CASE("Stochastic tunnleing")
{
    using trompeloeil::_;
    using namespace angonoka::stun;

    std::vector<int16> best_state{0, 0};

    RandomUtilsMock random_utils;
    MakespanEstimatorMock estimator;

    trompeloeil::sequence seq;

    REQUIRE_CALL(estimator, call(_))
        .WITH(static_cast<gsl::index>(_1.size()) == best_state.size())
        .RETURN(1.F)
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(random_utils, get_neighbor_inplace(_))
        .WITH(static_cast<gsl::index>(_1.size()) == best_state.size())
        .SIDE_EFFECT(ranges::copy(std::vector{1, 1}, _1.begin()))
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(estimator, call(_)).RETURN(1.F).IN_SEQUENCE(seq);

    REQUIRE_CALL(random_utils, get_uniform())
        .RETURN(1.F)
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(random_utils, get_neighbor_inplace(_))
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(estimator, call(_)).RETURN(.5F).IN_SEQUENCE(seq);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
    const auto result = stochastic_tunneling(
        random_utils,
        estimator,
        best_state,
        Gamma{.5F},
        Beta{1.F},
        BetaScale{.3F});
#pragma clang diagnostic pop

    REQUIRE(result.energy == Approx(.5F));
    REQUIRE(result.state == std::vector<int16>{1, 1});
}
