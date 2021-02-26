#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka {
struct Configuration;
} // namespace angonoka

namespace angonoka::stun {
using ranges::span;

/**
    Cache-friendly container of views into an array of ints.
*/
class Vector2D {
public:
    /**
        Default constructor.
    */
    Vector2D() noexcept;

    /**
        Constructor.

        @param data     Array of ints
        @param spans    Array of spans
    */
    Vector2D(
        std::vector<int16>&& data,
        std::vector<span<int16>>&& spans) noexcept;

    /**
        Construct from an array of span sizes.

        @param data     Array of ints
        @param sizes    Array of span sizes
    */
    Vector2D(
        std::vector<int16>&& data,
        span<const int16> sizes) noexcept;

    Vector2D(const Vector2D& other);
    Vector2D& operator=(const Vector2D& other);
    Vector2D(Vector2D&& other) noexcept;
    Vector2D& operator=(Vector2D&& other) noexcept;
    ~Vector2D() noexcept;

    /**
        Get a span by index.

        @param index Span index

        @return A span of ints.
    */
    template <typename T> decltype(auto) operator[](T&& index) const
    {
        Expects(!spans.empty());
        return spans[std::forward<T>(index)];
    }

    /**
        Clear the contents of the container.
    */
    void clear() noexcept;

    /**
        Get size of the container.

        @return Size of the container.
    */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
        Check if the container is empty.

        @return True if the container is empty.
    */
    [[nodiscard]] bool empty() const noexcept;

private:
    std::vector<int16> data;
    std::vector<span<int16>> spans;
};

/**
    General, read-only information about the schedule.

    @var agent_performance      Agent's speed multipliers
    @var task_duration          Task durations in seconds
    @var available_agents       Which agents can perform each task
    @var dependencies           Task's dependent sub-tasks
*/
struct ScheduleInfo {
    std::vector<float> agent_performance;
    std::vector<float> task_duration;
    Vector2D available_agents;
    Vector2D dependencies;
};

/**
    Construct a valid but naive schedule.

    @param ScheduleInfo An instance of ScheduleInfo

    @return A valid schedule
*/
std::vector<StateItem> initial_state(const ScheduleInfo& info);

/**
    Construct ScheduleInfo from Configuration.

    TODO: test

    @param config An instance of Configuration

    @return ScheduleInfo
*/
ScheduleInfo to_schedule(const Configuration& config);
} // namespace angonoka::stun
