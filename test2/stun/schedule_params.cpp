#include "stun/schedule_params.h"
#include "config/load.h"

#include <catch2/catch.hpp>

TEST_CASE("ScheduleParams") {
    SECTION("ScheduleParams type traits") {
        using angonoka::stun::ScheduleParams;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<ScheduleParams>);
        STATIC_REQUIRE(std::is_nothrow_default_constructible_v<ScheduleParams>);
        STATIC_REQUIRE(std::is_copy_constructible_v<ScheduleParams>);
        STATIC_REQUIRE(std::is_copy_assignable_v<ScheduleParams>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<ScheduleParams>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ScheduleParams>);
    }

    SECTION("ScheduleParams special memeber functions") {
        using namespace angonoka::stun;

            ScheduleParams params{
                .agent_performance{1.F, 2.F, 3.F},
                .task_duration{3.F, 2.F, 1.F}};

            {
                std::vector<int16>
                    available_agents_data{2, 1, 2, 0, 1, 2};
                auto* p = available_agents_data.data();
                const auto n = [&](auto s) -> span<int16> {
                    return {std::exchange(p, std::next(p, s)), s};
                };
                std::vector<span<int16>> available_agents
                    = {n(1), n(2), n(3)};
                params.available_agents
                    = {std::move(available_agents_data),
                       std::move(available_agents)};
            }
            {
                std::vector<int16> dependencies_data{0, 0, 1};
                auto* p = dependencies_data.data();
                const auto n = [&](auto s) -> span<int16> {
                    return {std::exchange(p, std::next(p, s)), s};
                };
                std::vector<span<int16>> dependencies
                    = {n(0), n(1), n(2)};
                params.dependencies
                    = {std::move(dependencies_data),
                       std::move(dependencies)};
            }

        SECTION("move ctor") {
            ScheduleParams other{std::move(params)};

            REQUIRE(params.dependencies.empty());
            REQUIRE(!other.dependencies.empty());
            REQUIRE(other.dependencies[2u][1] == 1);
        }

        SECTION("move assignment") {
            ScheduleParams other;
            other = std::move(params);

            REQUIRE(params.dependencies.empty());
            REQUIRE(!other.dependencies.empty());
            REQUIRE(other.dependencies[2u][1] == 1);
        }

        SECTION("copy ctor") {
            ScheduleParams other{params};

            params.dependencies.clear();

            REQUIRE(other.dependencies[2u][1] == 1);
        }

        SECTION("copy assignment") {
            ScheduleParams other;
            other = params;

            params.dependencies.clear();

            REQUIRE(other.dependencies[2u][1] == 1);
        }

        SECTION("self copy") {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            params = params;
#pragma clang diagnostic pop

            REQUIRE(params.dependencies[2u][1] == 1);
        }

        SECTION("self move") {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            params = std::move(params);
#pragma clang diagnostic pop

            REQUIRE(params.dependencies[2u][1] == 1);
        }
    }

    SECTION("Vector2D type traits") {
        using angonoka::stun::Vector2D;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<Vector2D>);
        STATIC_REQUIRE(std::is_nothrow_default_constructible_v<Vector2D>);
        STATIC_REQUIRE(std::is_copy_constructible_v<Vector2D>);
        STATIC_REQUIRE(std::is_copy_assignable_v<Vector2D>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Vector2D>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Vector2D>);
    }

    SECTION("Vector2D special memeber functions") {
        using namespace angonoka::stun;

        SECTION("empty") {
            Vector2D vspans;

            REQUIRE(vspans.empty());

            SECTION("copy ctor") {
                Vector2D other{vspans};

                REQUIRE(other.empty());
            }
        }

        SECTION("empty input arrays") {
            Vector2D vspans{
                std::vector<int16>{},
                span<const int16>{}};

            REQUIRE(vspans.empty());
        }

        SECTION("non-empty") {
                std::vector<int16> data{0, 1, 2};
                auto* b = data.data();
                const auto f = [&](auto s) -> span<int16> {
                    return {std::exchange(b, std::next(b, s)), s};
                };
                std::vector<span<int16>> spans{f(1), f(1), f(1)};

            SECTION("constructor") {
                Vector2D vspans{std::move(data), std::move(spans)};

                REQUIRE(vspans.size() == 3);
            }

            SECTION("copy ctor") {
                Vector2D vspans{std::move(data), std::move(spans)};
                Vector2D other{vspans};
                vspans.clear();

                REQUIRE(other.size() == 3);
                REQUIRE(other[2u][0] == 2);
            }

            SECTION("copy assignment") {
                Vector2D vspans{std::move(data), std::move(spans)};
                Vector2D other;
                other = vspans;
                vspans.clear();

                REQUIRE(other.size() == 3);
                REQUIRE(other[2u][0] == 2);

                other = vspans;

                REQUIRE(other.empty());
            }

            SECTION("move ctor") {
                Vector2D vspans{std::move(data), std::move(spans)};
                Vector2D other{std::move(vspans)};

                REQUIRE(vspans.empty());
                REQUIRE(other.size() == 3);
                REQUIRE(other[2u][0] == 2);
            }

            SECTION("move assignment"){
                Vector2D vspans{std::move(data), std::move(spans)};
                Vector2D other;
                other = std::move(vspans);

                REQUIRE(vspans.empty());
                REQUIRE(other.size() == 3);
                REQUIRE(other[2u][0] == 2);
            }

            SECTION("self copy"){
                Vector2D vspans{std::move(data), std::move(spans)};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
                vspans = vspans;
#pragma clang diagnostic pop

                REQUIRE(vspans[2u][0] == 2);
            }

            SECTION("self move") {
                Vector2D vspans{std::move(data), std::move(spans)};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
                vspans = std::move(vspans);
#pragma clang diagnostic pop

                REQUIRE(vspans[2u][0] == 2);
            }
        }

        SECTION("construct from array of sizes") {
            std::vector<int16> data{1, 2, 3, 4, 5, 6};
            const std::vector<int16> sizes{1, 2, 3};

            const Vector2D vec{std::move(data), sizes};

            REQUIRE(vec.size() == 3);
            REQUIRE(vec[1u].size() == 2);
            REQUIRE(vec[1u][1] == 3);
        }
    }

    SECTION("initial schedule") {
        using namespace angonoka::stun;

        const ScheduleParams params{
            .agent_performance{1.F, 1.F, 1.F},
            .task_duration{1.F, 1.F, 1.F, 1.F, 1.F, 1.F},
            .available_agents{
                std::vector<int16>{0, 1, 2, 0, 1, 2},
                std::vector<int16>{1, 1, 1, 1, 1, 1}},
            .dependencies{
                std::vector<int16>{1, 2, 3, 4, 5},
                std::vector<int16>{1, 1, 1, 1, 1, 0}}};

        const auto schedule = initial_schedule(params);

        REQUIRE(
            schedule
            == std::vector<ScheduleItem>{
                {.taskd = 5, .agentd = 2},
                {.taskd = 4, .agentd = 1},
                {.taskd = 3, .agentd = 0},
                {.taskd = 2, .agentd = 2},
                {.taskd = 1, .agentd = 1},
                {.taskd = 0, .agentd = 0}});
    }

    SECTION("ScheduleParams from Configuration") {
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

        const auto params = to_schedule_params(config);

        REQUIRE(params.agent_performance.size() == 2);
        REQUIRE(params.agent_performance[0] == Approx(1.));
        REQUIRE(params.task_duration.size() == 2);
        REQUIRE(params.task_duration[0] == Approx(1.));
        REQUIRE(params.available_agents.size() == 2);
        // Both agents
        REQUIRE(params.available_agents[0u].size() == 2);
        // Only Jack due to group A constraint
        REQUIRE(params.available_agents[1u].size() == 1);
        REQUIRE(params.dependencies.size() == 2);
        REQUIRE(params.dependencies[0u].empty());
        REQUIRE(params.dependencies[1u].size() == 1);
        REQUIRE(params.dependencies[1u][0] == 0);
    }
}
