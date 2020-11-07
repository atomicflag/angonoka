#include "stun/makespan_estimator.h"
#include "stun/task_duration_cache.h"
#include "utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <vector>

namespace {
using namespace angonoka::stun;
struct TaskDurationCacheMock final : TaskDurationCacheStub {
    MAKE_CONST_MOCK2(
        get,
        float(AgentIndex, TaskIndex),
        noexcept override);
};
} // namespace

TEST_CASE("MakespanEstimator type traits")
{
    using angonoka::stun::MakespanEstimator;
    static_assert(std::is_nothrow_destructible_v<MakespanEstimator>);
    static_assert(
        !std::is_default_constructible_v<MakespanEstimator>);
    static_assert(std::is_copy_constructible_v<MakespanEstimator>);
    static_assert(std::is_copy_assignable_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_constructible_v<MakespanEstimator>);
    static_assert(
        std::is_nothrow_move_assignable_v<MakespanEstimator>);
}

TEST_CASE("MakespanEstimator estimation")
{
    using namespace angonoka::stun;
    using angonoka::utils::make_array;
    using trompeloeil::_;

    {
        const TaskDurationCacheMock cache;

        REQUIRE_CALL(cache, get(AgentIndex{1}, TaskIndex{0}))
            .RETURN(1.F);
        REQUIRE_CALL(cache, get(AgentIndex{2}, TaskIndex{1}))
            .RETURN(1.F);
        REQUIRE_CALL(cache, get(AgentIndex{1}, TaskIndex{2}))
            .RETURN(1.F);

        MakespanEstimator estimator{3, &cache};

        REQUIRE(estimator(make_array<int16>(1, 2, 1)) == 2.0F);
    }
    {
        const TaskDurationCacheMock cache;

        REQUIRE_CALL(cache, get(AgentIndex{0}, TaskIndex{0}))
            .RETURN(1.F);
        REQUIRE_CALL(cache, get(AgentIndex{1}, TaskIndex{1}))
            .RETURN(1.F);
        REQUIRE_CALL(cache, get(AgentIndex{2}, TaskIndex{2}))
            .RETURN(1.F);

        MakespanEstimator estimator{3, &cache};

        REQUIRE(estimator(make_array<int16>(0, 1, 2)) == 1.0F);
    }
    {
        const TaskDurationCacheMock cache;

        REQUIRE_CALL(cache, get(AgentIndex{2}, TaskIndex{0}))
            .RETURN(3.F);
        REQUIRE_CALL(cache, get(AgentIndex{1}, TaskIndex{1}))
            .RETURN(1.F);
        REQUIRE_CALL(cache, get(AgentIndex{0}, TaskIndex{2}))
            .RETURN(1.F);

        MakespanEstimator estimator{3, &cache};

        REQUIRE(estimator(make_array<int16>(2, 1, 0)) == 3.0F);
    }
}
