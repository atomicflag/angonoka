#include "predict.h"
#include "configuration.h"
#include "stub/optimizer.h"
#include "stub/schedule_params.h"
#include <boost/ut.hpp>
#include <vector>

using namespace boost::ut;

namespace angonoka::stun {
ScheduleParams to_schedule_params(const Configuration&)
{
    return {
        .agent_performance{1.f},
        .task_duration{1.f},
        .available_agents{1.f},
        .dependencies{1.f}};
}

bool Optimizer::has_converged() { return steps >= 5; }
void Optimizer::update() { steps += 1; }
State Optimizer::state() { return {}; }
float Optimizer::estimated_progress()
{
    return static_cast<float>(steps) / 5.f;
}
} // namespace angonoka::stun

suite predict_test = [] {
    "basic prediction"_test = [] {
        using namespace angonoka;

        Configuration config;
        auto [prediction_future, event_queue] = predict(config);
        prediction_future.get();

        std::vector<ProgressEvent> events;
        for (ProgressEvent evt; event_queue->try_dequeue(evt);)
            events.emplace_back(std::move(evt));

        expect(events.size() == 7_i);
        expect(
            get<SimpleProgressEvent>(events[0])
            == SimpleProgressEvent::Start);
        expect(
            get<ScheduleOptimizationEvent>(events[1]).progress
            == .2_d);
        expect(
            get<ScheduleOptimizationEvent>(events[2]).progress
            == .4_d);
        expect(
            get<ScheduleOptimizationEvent>(events[3]).progress
            == .6_d);
        expect(
            get<ScheduleOptimizationEvent>(events[4]).progress
            == .8_d);
        expect(
            get<ScheduleOptimizationEvent>(events[5]).progress
            == 1._d);
        expect(
            get<SimpleProgressEvent>(events[6])
            == SimpleProgressEvent::Done);
        // TODO: implement
    };
};
