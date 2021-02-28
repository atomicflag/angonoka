#include "stun/schedule_info.h"
#include "config/load.h"
#include <catch2/catch.hpp>

TEST_CASE("ScheduleInfo type traits")
{
    using angonoka::stun::ScheduleInfo;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<ScheduleInfo>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_copy_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_copy_assignable_v<ScheduleInfo>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ScheduleInfo>);
}

TEST_CASE("ScheduleInfo special memeber functions")
{
    using namespace angonoka::stun;

    ScheduleInfo info{
        .agent_performance{1.F, 2.F, 3.F},
        .task_duration{3.F, 2.F, 1.F}};

    {
        std::vector<int16> available_agents_data{2, 1, 2, 0, 1, 2};
        auto* p = available_agents_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        std::vector<span<int16>> available_agents
            = {n(1), n(2), n(3)};
        info.available_agents
            = {std::move(available_agents_data),
               std::move(available_agents)};
    }
    {
        std::vector<int16> dependencies_data{0, 0, 1};
        auto* p = dependencies_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        std::vector<span<int16>> dependencies = {n(0), n(1), n(2)};
        info.dependencies
            = {std::move(dependencies_data), std::move(dependencies)};
    }

    SECTION("Move ctor")
    {
        ScheduleInfo other{std::move(info)};

        REQUIRE(info.dependencies.empty());
        REQUIRE_FALSE(other.dependencies.empty());
        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Move assignment")
    {
        ScheduleInfo other;
        other = std::move(info);

        REQUIRE(info.dependencies.empty());
        REQUIRE_FALSE(other.dependencies.empty());
        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Copy ctor")
    {
        ScheduleInfo other{info};

        info.dependencies.clear();

        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Copy assignment")
    {
        ScheduleInfo other;
        other = info;

        info.dependencies.clear();

        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Self copy")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
        info = info;
#pragma clang diagnostic pop

        REQUIRE(info.dependencies[2u][1] == 1);
    }

    SECTION("Self move")
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
        info = std::move(info);
#pragma clang diagnostic pop

        REQUIRE(info.dependencies[2u][1] == 1);
    }
}

TEST_CASE("Vector2D type traits")
{
    using angonoka::stun::Vector2D;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Vector2D>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<Vector2D>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Vector2D>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Vector2D>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Vector2D>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Vector2D>);
}

TEST_CASE("Vector2D special memeber functions")
{
    using namespace angonoka::stun;

    SECTION("Empty")
    {
        Vector2D vspans;

        REQUIRE(vspans.empty());

        SECTION("Copy ctor")
        {
            Vector2D other{vspans};

            REQUIRE(other.empty());
        }
    }

    SECTION("Non-empty")
    {

        std::vector<int16> data{0, 1, 2};
        auto* b = data.data();
        const auto f = [&](auto s) -> span<int16> {
            return {std::exchange(b, std::next(b, s)), s};
        };
        std::vector<span<int16>> spans{f(1), f(1), f(1)};

        Vector2D vspans{std::move(data), std::move(spans)};

        REQUIRE(vspans.size() == 3);

        SECTION("Copy ctor")
        {
            Vector2D other{vspans};
            vspans.clear();

            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Copy assignment")
        {
            Vector2D other;
            other = vspans;
            vspans.clear();

            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);

            other = vspans;

            REQUIRE(other.empty());
        }

        SECTION("Move ctor")
        {
            Vector2D other{std::move(vspans)};

            REQUIRE(vspans.empty());
            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Move assignment")
        {
            Vector2D other;
            other = std::move(vspans);

            REQUIRE(vspans.empty());
            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            vspans = vspans;
#pragma clang diagnostic pop

            REQUIRE(vspans[2u][0] == 2);
        }

        SECTION("Self move")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            vspans = std::move(vspans);
#pragma clang diagnostic pop

            REQUIRE(vspans[2u][0] == 2);
        }
    }

    SECTION("Construct from array of sizes")
    {
        std::vector<int16> data{1, 2, 3, 4, 5, 6};
        const std::vector<int16> sizes{1, 2, 3};

        const Vector2D vec{std::move(data), sizes};

        REQUIRE(vec.size() == 3);
        REQUIRE(vec[1u].size() == 2);
        REQUIRE(vec[1u][1] == 3);
    }
}

TEST_CASE("Initial state")
{
    using namespace angonoka::stun;

    const ScheduleInfo info{
        .agent_performance{1.F, 1.F, 1.F},
        .task_duration{1.F, 1.F, 1.F, 1.F, 1.F, 1.F},
        .available_agents{
            std::vector<int16>{0, 1, 2, 0, 1, 2},
            std::vector<int16>{1, 1, 1, 1, 1, 1}},
        .dependencies{
            std::vector<int16>{1, 2, 3, 4, 5},
            std::vector<int16>{1, 1, 1, 1, 1, 0}}};

    const auto state = initial_state(info);

    REQUIRE(
        state
        == std::vector<StateItem>{
            {.task_id = 5, .agent_id = 2},
            {.task_id = 4, .agent_id = 1},
            {.task_id = 3, .agent_id = 0},
            {.task_id = 2, .agent_id = 2},
            {.task_id = 1, .agent_id = 1},
            {.task_id = 0, .agent_id = 0}});
}

TEST_CASE("ScheduleInfo from Configuration")
{
    using namespace angonoka::stun;
    // clang-format off
    constexpr auto text = 
        "agents:\n"
        "  Bob:\n"
        "    performance:\n"
        "      min: 0.5\n"
        "      max: 1.5\n"
        "  Jack:\n"
        "    groups:\n"
        "      - A\n"
        "tasks:\n"
        "  - name: Task 1\n"
        "    duration: 1h\n"
        "    id: 1\n"
        "    group: A\n"
        "  - name: Task 2\n"
        "    duration: 1h\n"
        "    depends_on: 1";
    // clang-format on
    const auto config = angonoka::load_text(text);

    const auto info = to_schedule_info(config);

    REQUIRE(info.agent_performance.size() == 2);
    REQUIRE(info.agent_performance[0] == Approx(1.F));
    REQUIRE(info.task_duration.size() == 2);
    REQUIRE(info.task_duration[0] == Approx(1.F));
    REQUIRE(info.available_agents.size() == 2);
    // Both agents
    REQUIRE(info.available_agents[0u].size() == 2);
    // Only Jack due to group A constraint
    REQUIRE(info.available_agents[1u].size() == 1);
    REQUIRE(info.dependencies.size() == 2);
    REQUIRE(info.dependencies[0u].empty());
    REQUIRE(info.dependencies[1u].size() == 1);
    REQUIRE(info.dependencies[1u][0] == 0);
}
