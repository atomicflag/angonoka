#pragma once

// Forward declaration
namespace angonoka {
struct System;
} // namespace angonoka

// Forward declaration
namespace YAML {
class Node;
} // namespace YAML

namespace angonoka::detail {
void validate_tasks(const YAML::Node& node);
} // namespace angonoka::detail
