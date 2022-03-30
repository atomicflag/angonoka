#include "predict.h"
#include "configuration.h"
#include "stub/optimizer.h"
#include "stub/schedule_params.h"
#include "stub/simulation.h"
#include <catch2/catch.hpp>
#include <deque>

namespace angonoka::stun {
ScheduleParams to_schedule_params(const Configuration&)
{
    return {
        .agent_performance{1.f},
        .task_duration{1.f},
        .available_agents{1.f},
        .dependencies{1.f},
        .duration_multiplier{10.f}};
}

bool Optimizer::has_converged() { return steps >= 5; }
void Optimizer::update() { steps += 1; }
Schedule Optimizer::schedule()
{
    static const std::vector<ScheduleItem> data{{0, 0}, {4, 2}};
    return data;
}
float Optimizer::estimated_progress()
{
    return static_cast<float>(steps) / 5.f;
}
float Optimizer::normalized_makespan() const
{
    return 5.f / static_cast<float>(steps);
}
int Optimizer::current_epoch() const { return steps; }
} // namespace angonoka::stun

namespace {
template <typename T> auto pop(auto& events)
{
    auto v = get<T>(events.front());
    events.pop_front();
    return v;
}
} // namespace

namespace angonoka {
[[nodiscard]] Histogram
histogram(const Configuration&, const OptimizedSchedule&)
{
    return {};
}
HistogramStats stats(const Histogram&)
{
    return {std::chrono::seconds{25}};
}
} // namespace angonoka

TEST_CASE("prediction")
{
    SECTION("basic prediction")
    {
        using namespace angonoka;
        using namespace std::literals::chrono_literals;

        Configuration config;
        auto [prediction_future, event_queue] = predict(config);
        const auto prediction_result = prediction_future.get();

        REQUIRE(
            prediction_result.stats.p25 == std::chrono::seconds{25});

        std::deque<ProgressEvent> events;
        for (ProgressEvent evt; event_queue->try_dequeue(evt);)
            events.emplace_back(std::move(evt));

        REQUIRE(
            pop<SimpleProgressEvent>(events)
            == SimpleProgressEvent::ScheduleOptimizationStart);
        {
            const auto evt = pop<ScheduleOptimizationEvent>(events);
            REQUIRE(evt.progress == Approx(.2));
            REQUIRE(evt.makespan == 50s);
            REQUIRE(evt.current_epoch == 1);
        }
        REQUIRE(
            pop<ScheduleOptimizationEvent>(events).progress
            == Approx(.4));
        REQUIRE(
            pop<ScheduleOptimizationEvent>(events).progress
            == Approx(.6));
        REQUIRE(
            pop<ScheduleOptimizationEvent>(events).progress
            == Approx(.8));
        {
            const auto evt = pop<ScheduleOptimizationEvent>(events);
            REQUIRE(evt.progress == Approx(1.));
            REQUIRE(evt.makespan == 10s);
            REQUIRE(evt.current_epoch == 5);
        }
        {
            const auto evt
                = pop<ScheduleOptimizationComplete>(events);
            REQUIRE(evt.makespan == 10s);
        }
        REQUIRE(
            pop<SimpleProgressEvent>(events)
            == SimpleProgressEvent::SimulationStart);
        REQUIRE(
            pop<SimpleProgressEvent>(events)
            == SimpleProgressEvent::Finished);
        REQUIRE(events.empty());
    }

    SECTION("a schedule without prediction")
    {
        using namespace angonoka;
        using namespace std::literals::chrono_literals;

        Configuration config;
        auto [prediction_future, event_queue] = schedule(config);
        const auto r = prediction_future.get();

        REQUIRE_FALSE(r.schedule.empty());
        REQUIRE(r.makespan == 10s);
        REQUIRE(
            r.schedule
            == std::vector<stun::ScheduleItem>{{0, 0}, {4, 2}});
    }

    SECTION("events")
    {
        using boost::variant2::variant_alternative;
        using namespace angonoka;
        REQUIRE(std::is_same_v<
                variant_alternative<0, ProgressEvent>::type,
                SimpleProgressEvent>);
        REQUIRE(
            SimpleProgressEvent{} != SimpleProgressEvent::Finished);
    }
}
