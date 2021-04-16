#include <catch2/catch.hpp>

#include "configuration.h"
#include <boost/lockfree/queue.hpp>
#include <future>
#include <memory>
#include <tuple>

namespace angonoka {
// TODO: doc, test, expects
struct Prediction {
};

template <typename... Ts> using Queue = boost::lockfree::queue<Ts...>;

// TODO: doc, test, expects
enum class ProgressEventType { Start, ScheduleOptimization, Done };

// TODO: doc, test, expects
struct ProgressEvent {
    ProgressEventType type;
};

// TODO: doc, test, expects
std::tuple<
    std::future<Prediction>,
    std::shared_ptr<Queue<ProgressEvent>>>
predict(const Configuration&)
{

    auto queue = std::make_shared<Queue<ProgressEvent>>(100);
    auto future = std::async(std::launch::async, [=] {
        return Prediction{};
    });
    return {std::move(future), std::move(queue)};
}
} // namespace angonoka

TEST_CASE("Basic prediction")
{
    using namespace angonoka;

    Configuration config;
    auto [prediction_future, queue] = predict(config);
    prediction_future.get();

    // TODO: implement
}
