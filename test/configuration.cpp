#include "configuration.h"
#include <boost/ut.hpp>
#include <type_traits>

using namespace boost::ut;

suite configuration = [] {
    "configuration type traits"_test = [] {
        using angonoka::Configuration;
        expect(std::is_nothrow_destructible_v<Configuration>);
        expect(
            std::is_nothrow_default_constructible_v<Configuration>);
        expect(std::is_copy_constructible_v<Configuration>);
        expect(std::is_copy_assignable_v<Configuration>);
        expect(std::is_move_constructible_v<Configuration>);
        expect(std::is_move_assignable_v<Configuration>);
    };

    "agent type traits"_test = [] {
        using angonoka::Agent;
        expect(std::is_nothrow_destructible_v<Agent>);
        expect(std::is_default_constructible_v<Agent>);
        expect(std::is_copy_constructible_v<Agent>);
        expect(std::is_copy_assignable_v<Agent>);
        expect(std::is_nothrow_move_constructible_v<Agent>);
        expect(std::is_nothrow_move_assignable_v<Agent>);
    };

    "task type traits"_test = [] {
        using angonoka::Task;
        expect(std::is_nothrow_destructible_v<Task>);
        expect(std::is_default_constructible_v<Task>);
        expect(std::is_copy_constructible_v<Task>);
        expect(std::is_copy_assignable_v<Task>);
        expect(std::is_nothrow_move_constructible_v<Task>);
        expect(std::is_nothrow_move_assignable_v<Task>);
    };

    "configuration utility functions"_test = [] {
        angonoka::Configuration s;

        s.groups.emplace_back("Test Group");
        auto& t = s.tasks.emplace_back();
        auto& a = s.agents.emplace_back();

        expect(has_universal_agents(s));
        expect(can_work_on(a, t));

        a.group_ids.emplace(angonoka::GroupIndex{0});

        expect(!has_universal_agents(s));
        expect(!can_work_on(a, t));

        t.group_ids.emplace(angonoka::GroupIndex{0});

        expect(can_work_on(a, t));
    };

    "agent utility functions"_test = [] {
        angonoka::Agent a;

        expect(is_universal(a));
        expect(can_work_on(a, angonoka::GroupIndex{0}));

        a.group_ids.emplace(angonoka::GroupIndex{1});

        expect(!is_universal(a));
        expect(!can_work_on(a, angonoka::GroupIndex{0}));
        expect(can_work_on(a, angonoka::GroupIndex{1}));
    };

    "agent member functions"_test = [] {
        angonoka::Agent a;

        expect(a.performance.average() == 1._d);

        a.performance.min = 0.05F;
        a.performance.max = 0.15F;

        expect(a.performance.average() == .1_d);
    };

    "task member functions"_test = [] {
        using namespace std::literals::chrono_literals;
        angonoka::Task t{.duration{.min{1s}, .max{3s}}};

        expect(t.duration.average() == 2s);
    };
};
