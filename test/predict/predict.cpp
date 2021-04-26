#include "config/load.h"
#include "configuration.h"
#include "stun/common.h"
#include <boost/ut.hpp>
#include <boost/variant.hpp>
#include <future>
#include <memory>
#include <range/v3/to_container.hpp>
#include <readerwriterqueue/readerwriterqueue.h>
#include <tuple>
#include <vector>

using namespace boost::ut;

namespace angonoka {

// Mocks start
enum class BatchSize : std::int_fast32_t;
enum class MaxIdleIters : std::int_fast32_t;
struct Optimizer {
    Optimizer(auto...) { }

    bool has_converged() { return steps >= 5; }
    void update() { steps += 1; }
    stun::State state() { return {}; }
    float estimated_progress()
    {
        return static_cast<float>(steps) / 5.f;
    }

    int steps{0};
};
namespace stun {
    struct ScheduleParams {
        std::vector<float> agent_performance;
        std::vector<float> task_duration;
        std::vector<float> available_agents;
        std::vector<float> dependencies;
    };

    ScheduleParams to_schedule_params(auto...)
    {
        return {
            .agent_performance{1.f},
            .task_duration{1.f},
            .available_agents{1.f},
            .dependencies{1.f}};
    }
} // namespace stun
// Mocks end

// TODO: doc, test, expects
struct Prediction {
};

template <typename... Ts>
using Queue = moodycamel::ReaderWriterQueue<Ts...>;

// TODO: doc, test, expects
enum class SimpleProgressEvent { Start, Done };

// TODO: doc, test, expects
struct ScheduleOptimizationEvent {
    float progress;
};

using ProgressEvent
    = boost::variant<SimpleProgressEvent, ScheduleOptimizationEvent>;

// TODO: doc, test, expects
std::vector<stun::StateItem> optimize(
    const stun::ScheduleParams& params,
    Queue<ProgressEvent>& events)
{
    Expects(!params.agent_performance.empty());
    Expects(!params.task_duration.empty());
    Expects(!params.available_agents.empty());
    Expects(
        params.available_agents.size()
        == params.task_duration.size());

    using namespace angonoka::stun;

    constexpr auto batch_size = 10'000;
    constexpr auto max_idle_iters = 1'000'000;

    Optimizer optimizer{
        params,
        BatchSize{batch_size},
        MaxIdleIters{max_idle_iters}};
    while (!optimizer.has_converged()) {
        optimizer.update();
        events.enqueue(ScheduleOptimizationEvent{
            .progress = optimizer.estimated_progress()});
    }

    return ranges::to<std::vector<StateItem>>(optimizer.state());
}

// TODO: doc, test, expects
std::tuple<
    std::future<Prediction>,
    std::shared_ptr<Queue<ProgressEvent>>>
predict(const Configuration& config)
{

    auto events = std::make_shared<Queue<ProgressEvent>>(100);
    auto future = std::async(std::launch::async, [=] {
        events->enqueue(SimpleProgressEvent::Start);
        // TODO: stub to_schedule, etc
        const auto schedule_params = stun::to_schedule_params(config);
        const auto state = optimize(schedule_params, *events);
        events->enqueue(SimpleProgressEvent::Done);
        return Prediction{};
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka

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
