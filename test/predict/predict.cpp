#include "predict.h"
#include "configuration.h"
#include "stub/optimizer.h"
#include "stub/schedule_params.h"
#include <boost/ut.hpp>
#include <deque>

using namespace boost::ut;

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
Schedule Optimizer::schedule() { return {}; }
float Optimizer::estimated_progress()
{
    return static_cast<float>(steps) / 5.f;
}
float Optimizer::normalized_makespan() const
{
    return 5.f / static_cast<float>(steps);
}
} // namespace angonoka::stun

namespace {
template <typename T> auto pop(auto& events)
{
    auto v = get<T>(events.front());
    events.pop_front();
    return v;
}
} // namespace

suite predict_test = [] {
    "basic prediction"_test = [] {
        using namespace angonoka;
        using namespace std::literals::chrono_literals;

        Configuration config;
        auto [prediction_future, event_queue] = predict(config);
        prediction_future.get();

        std::deque<ProgressEvent> events;
        for (ProgressEvent evt; event_queue->try_dequeue(evt);)
            events.emplace_back(std::move(evt));

        expect(
            pop<SimpleProgressEvent>(events)
            == SimpleProgressEvent::ScheduleOptimizationStart);
        {
            const auto evt = pop<ScheduleOptimizationEvent>(events);
            expect(evt.progress == .2_d);
            expect(evt.makespan == 50s);
        }
        expect(
            pop<ScheduleOptimizationEvent>(events).progress == .4_d);
        expect(
            pop<ScheduleOptimizationEvent>(events).progress == .6_d);
        expect(
            pop<ScheduleOptimizationEvent>(events).progress == .8_d);
        {
            const auto evt = pop<ScheduleOptimizationEvent>(events);
            expect(evt.progress == 1._d);
            expect(evt.makespan == 10s);
        }
        {
            const auto evt
                = pop<ScheduleOptimizationComplete>(events);
            expect(evt.makespan == 10s);
        }
        expect(
            pop<SimpleProgressEvent>(events)
            == SimpleProgressEvent::Finished);
        expect(events.empty());
        // TODO: implement
    };

    "events"_test = [] {
        using boost::variant2::variant_alternative;
        using namespace angonoka;
        expect(std::is_same_v<
               variant_alternative<0, ProgressEvent>::type,
               SimpleProgressEvent>);
        expect(
            SimpleProgressEvent{} != SimpleProgressEvent::Finished);
    };
};
