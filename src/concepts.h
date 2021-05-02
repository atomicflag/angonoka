#pragma once

#include <string_view>

namespace angonoka {
template <typename T>
concept String = requires(T a)
{
    std::string_view{a};
};
} // namespace angonoka
