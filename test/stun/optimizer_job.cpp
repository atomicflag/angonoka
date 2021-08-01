#include "stun/optimizer_job.h"
#include "config/load.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite optimizer_job = [] {
    "OptimizerJob type traits"_test = [] {
        using angonoka::stun::OptimizerJob;
        expect(std::is_nothrow_destructible_v<OptimizerJob>);
        expect(!std::is_default_constructible_v<OptimizerJob>);
        expect(std::is_copy_constructible_v<OptimizerJob>);
        expect(std::is_copy_assignable_v<OptimizerJob>);
        expect(std::is_nothrow_move_constructible_v<OptimizerJob>);
        expect(std::is_nothrow_move_assignable_v<OptimizerJob>);
    };

    "basic OptimizerJob operations"_test = [] {
        using namespace angonoka::stun;

        // clang-format off
        constexpr auto text = 
            "agents:\n"
            "  Bob:\n"
            "  Jack:\n"
            "tasks:\n"
            "  - name: Task 1\n"
            "    duration: 1h\n"
            "  - name: Task 2\n"
            "    duration: 1h";
        // clang-format on
        const auto config = angonoka::load_text(text);

        const auto params = to_schedule_params(config);
        OptimizerJob optimizer{params, BatchSize{5}};

        expect(optimizer.normalized_makespan() == 2.F);
        expect(optimizer.schedule()[1].agent_id == 0);

        optimizer.update();

        while (optimizer.normalized_makespan() != 1.F)
            optimizer.update();

        // Might be non-deterministic
        expect(optimizer.normalized_makespan() == 1.F);
        // Each task has a different agent
        expect(
            optimizer.schedule()[1].agent_id
            != optimizer.schedule()[0].agent_id);

        should("reset") = [&] {
            optimizer.reset();

            expect(optimizer.normalized_makespan() == 2.F);
        };

        should("rebind params") = [&] {
            expect(&optimizer.params() == &params);
            const auto params2 = params;
            optimizer.params(params2);
            expect(&optimizer.params() == &params2);

            while (optimizer.normalized_makespan() != 1.F)
                optimizer.update();
        };
    };

    "OptimizerJob special memeber functions"_test = [] {
        using namespace angonoka::stun;

        // clang-format off
        constexpr auto text = 
            "agents:\n"
            "  Bob:\n"
            "  Jack:\n"
            "tasks:\n"
            "  - name: Task 1\n"
            "    duration: 1h\n"
            "  - name: Task 2\n"
            "    duration: 1h";
        // clang-format on
        const auto config = angonoka::load_text(text);

        const auto params = to_schedule_params(config);

        should("copy ctor") = [&] {
            OptimizerJob job{params, BatchSize{5}};
            OptimizerJob other{job};

            expect(other.normalized_makespan() == 2.F);

            job.update();

            expect(other.normalized_makespan() == 2.F);
        };

        should("copy assignment") = [&] {
            OptimizerJob job{params, BatchSize{5}};
            OptimizerJob other{params, BatchSize{5}};
            other = job;

            expect(other.normalized_makespan() == 2.F);

            job.update();

            expect(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            expect(other.normalized_makespan() == 1.F);

            {
                OptimizerJob job2{params, BatchSize{5}};
                other = job2;
            }

            expect(other.normalized_makespan() == 2.F);
        };

        should("move ctor") = [&] {
            OptimizerJob job{params, BatchSize{5}};
            OptimizerJob other{std::move(job)};

            expect(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            expect(other.normalized_makespan() == 1.F);
        };

        should("move assignment") = [&] {
            OptimizerJob job{params, BatchSize{5}};
            OptimizerJob other{params, BatchSize{5}};
            other = std::move(job);

            expect(other.normalized_makespan() == 2.F);

            while (other.normalized_makespan() != 1.F) other.update();

            expect(other.normalized_makespan() == 1.F);
        };

        should("destructive move assignment") = [&] {
            OptimizerJob job{params, BatchSize{5}};

            expect(job.normalized_makespan() == 2.F);

            {
                OptimizerJob other{params, BatchSize{5}};
                job = std::move(other);
            }

            expect(job.normalized_makespan() == 2.F);

            while (job.normalized_makespan() != 1.F) job.update();

            expect(job.normalized_makespan() == 1.F);
        };

        should("self copy") = [&] {
            OptimizerJob job{params, BatchSize{5}};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            job = job;
#pragma clang diagnostic pop

            expect(job.normalized_makespan() == 2.F);
        };

        should("self move") = [&] {
            OptimizerJob job{params, BatchSize{5}};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            job = std::move(job);
#pragma clang diagnostic pop

            expect(job.normalized_makespan() == 2.F);
        };
    };
};
