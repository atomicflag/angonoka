// TODO: remove this
#undef UNIT_TEST
#include <catch2/catch.hpp>

#include "config/load.h"
#include "configuration.h"
#include "stun/optimizer.h"
#include <readerwriterqueue/readerwriterqueue.h>
#include <future>
#include <memory>
#include <range/v3/to_container.hpp>
#include <tuple>
#include <vector>
#include <boost/variant.hpp>

namespace angonoka {
// TODO: doc, test, expects
struct Prediction {
};

template <typename... Ts> using Queue = moodycamel::ReaderWriterQueue<Ts...>;

// TODO: doc, test, expects
enum class SimpleProgressEvent { Start, Done };

// TODO: doc, test, expects
struct ScheduleOptimizationEvent {
    float progress;
};

using ProgressEvent = boost::variant<SimpleProgressEvent, ScheduleOptimizationEvent>;

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

    // TODO: Dependency inversion?
    Optimizer optimizer{
        params,
        BatchSize{batch_size},
        MaxIdleIters{max_idle_iters}};
    while (!optimizer.has_converged()) {
        optimizer.update();
        // fmt::print("{}\n", optimizer.estimated_progress());
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
        const auto schedule_params = stun::to_schedule_params(config);
        const auto state = optimize(schedule_params, *events);
        return Prediction{};
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka

TEST_CASE("Basic prediction")
{
    using namespace angonoka;

    Configuration config;
    auto [prediction_future, events] = predict(config);
    prediction_future.get();

    REQUIRE(events->size_approx() == 1);
    // TODO: implement
}
