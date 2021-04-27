#pragma once

#include "configuration.h"
#include <boost/variant.hpp>
#include <future>
#include <memory>
#include <readerwriterqueue/readerwriterqueue.h>
#include <tuple>

namespace angonoka {
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
std::tuple<
    std::future<Prediction>,
    std::shared_ptr<Queue<ProgressEvent>>>
predict(const Configuration& config);
} // namespace angonoka
