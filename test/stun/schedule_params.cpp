#include "stun/schedule_params.h"
#include "config/load.h"

#include <boost/ut.hpp>

using namespace boost::ut;

suite schedule_params = [] {
    "ScheduleParams type traits"_test = [] {
        using angonoka::stun::ScheduleParams;
        expect(std::is_nothrow_destructible_v<ScheduleParams>);
        expect(
            std::is_nothrow_default_constructible_v<ScheduleParams>);
        expect(std::is_copy_constructible_v<ScheduleParams>);
        expect(std::is_copy_assignable_v<ScheduleParams>);
        expect(std::is_nothrow_move_constructible_v<ScheduleParams>);
        expect(std::is_nothrow_move_assignable_v<ScheduleParams>);
    };

    "ScheduleParams special memeber functions"_test = [] {
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

        should("move ctor") = [=]() mutable {
            ScheduleParams other{std::move(params)};

            expect(params.dependencies.empty());
            expect(!other.dependencies.empty());
            expect(other.dependencies[2u][1] == 1);
        };

        should("move assignment") = [=]() mutable {
            ScheduleParams other;
            other = std::move(params);

            expect(params.dependencies.empty());
            expect(!other.dependencies.empty());
            expect(other.dependencies[2u][1] == 1);
        };

        should("copy ctor") = [=]() mutable {
            ScheduleParams other{params};

            params.dependencies.clear();

            expect(other.dependencies[2u][1] == 1);
        };

        should("copy assignment") = [=]() mutable {
            ScheduleParams other;
            other = params;

            params.dependencies.clear();

            expect(other.dependencies[2u][1] == 1);
        };

        should("self copy") = [=]() mutable {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            params = params;
#pragma clang diagnostic pop

            expect(params.dependencies[2u][1] == 1);
        };

        should("self move") = [=]() mutable {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            params = std::move(params);
#pragma clang diagnostic pop

            expect(params.dependencies[2u][1] == 1);
        };
    };

    "Vector2D type traits"_test = [] {
        using angonoka::stun::Vector2D;
        expect(std::is_nothrow_destructible_v<Vector2D>);
        expect(std::is_nothrow_default_constructible_v<Vector2D>);
        expect(std::is_copy_constructible_v<Vector2D>);
        expect(std::is_copy_assignable_v<Vector2D>);
        expect(std::is_nothrow_move_constructible_v<Vector2D>);
        expect(std::is_nothrow_move_assignable_v<Vector2D>);
    };

    "Vector2D special memeber functions"_test = [] {
        using namespace angonoka::stun;

        "empty"_test = [] {
            Vector2D vspans;

            expect(vspans.empty());

            should("copy ctor") = [=]() mutable {
                Vector2D other{vspans};

                expect(other.empty());
            };
        };

        "non-empty"_test = [] {
            std::vector<int16> data{0, 1, 2};
            auto* b = data.data();
            const auto f = [&](auto s) -> span<int16> {
                return {std::exchange(b, std::next(b, s)), s};
            };
            std::vector<span<int16>> spans{f(1), f(1), f(1)};

            Vector2D vspans{std::move(data), std::move(spans)};

            expect(vspans.size() == 3_i);

            should("copy ctor") = [=]() mutable {
                Vector2D other{vspans};
                vspans.clear();

                expect(other.size() == 3_i);
                expect(other[2u][0] == 2);
            };

            should("copy assignment") = [=]() mutable {
                Vector2D other;
                other = vspans;
                vspans.clear();

                expect(other.size() == 3_i);
                expect(other[2u][0] == 2);

                other = vspans;

                expect(other.empty());
            };

            should("move ctor") = [=]() mutable {
                Vector2D other{std::move(vspans)};

                expect(vspans.empty());
                expect(other.size() == 3_i);
                expect(other[2u][0] == 2);
            };

            should("move assignment") = [=]() mutable {
                Vector2D other;
                other = std::move(vspans);

                expect(vspans.empty());
                expect(other.size() == 3_i);
                expect(other[2u][0] == 2);
            };

            should("self copy") = [=]() mutable {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
                vspans = vspans;
#pragma clang diagnostic pop

                expect(vspans[2u][0] == 2);
            };

            should("self move") = [=]() mutable {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
                vspans = std::move(vspans);
#pragma clang diagnostic pop

                expect(vspans[2u][0] == 2);
            };
        };

        "construct from array of sizes"_test = [] {
            std::vector<int16> data{1, 2, 3, 4, 5, 6};
            const std::vector<int16> sizes{1, 2, 3};

            const Vector2D vec{std::move(data), sizes};

            expect(vec.size() == 3_i);
            expect(vec[1u].size() == 2_i);
            expect(vec[1u][1] == 3);
        };
    };

    "initial state"_test = [] {
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

        const auto state = initial_state(params);

        expect(
            state
            == std::vector<StateItem>{
                {.task_id = 5, .agent_id = 2},
                {.task_id = 4, .agent_id = 1},
                {.task_id = 3, .agent_id = 0},
                {.task_id = 2, .agent_id = 2},
                {.task_id = 1, .agent_id = 1},
                {.task_id = 0, .agent_id = 0}});
    };

    "ScheduleParams from Configuration"_test = [] {
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

        expect(params.agent_performance.size() == 2_i);
        expect(params.agent_performance[0] == 1._d);
        expect(params.task_duration.size() == 2_i);
        expect(params.task_duration[0] == 1._d);
        expect(params.available_agents.size() == 2_i);
        // Both agents
        expect(params.available_agents[0u].size() == 2_i);
        // Only Jack due to group A constraint
        expect(params.available_agents[1u].size() == 1_i);
        expect(params.dependencies.size() == 2_i);
        expect(params.dependencies[0u].empty());
        expect(params.dependencies[1u].size() == 1_i);
        expect(params.dependencies[1u][0] == 0);
    };
};
