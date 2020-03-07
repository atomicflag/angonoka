#include "load.h"
#include "../common.h"
#include "validation.h"
#include <range/v3/algorithm/find.hpp>

namespace {
/**
  Matches YAML configuration against the schema.

  @param node Root YAML node
*/
void validate_configuration(const YAML::Node& node)
{
    using namespace angonoka::validation;
    // clang-format off
    constexpr auto schema = attributes(
        required("agents",
            map(attributes(
                optional("perf", attributes(
                    required("min", scalar()),
                    required("max", scalar())
                )),
                optional("groups", sequence(scalar()))
            ))
        ),
        required("tasks",
            map(attributes(
                optional("group", scalar()),
                required("days", attributes(
                    required("min", scalar()),
                    required("max", scalar())
                ))
            ))
        )
    );
    // clang-format on
    schema(node);
}
} // namespace

namespace angonoka {
System load_text(const char* text)
{
    const auto node = YAML::Load(text);
    validate_configuration(node);
    System system;
    detail::parse_agents(node["agents"], system);
    detail::parse_tasks(node["tasks"], system);
    return system;
}
} // namespace angonoka

namespace angonoka::detail {
std::pair<int, bool>
find_or_insert_group(Groups& groups, std::string_view group)
{
    if (const auto f = ranges::find(groups, group); f != groups.end())
        return {Int{std::distance(groups.begin(), f)}, false};
    groups.emplace_back(group);
    return {Int{groups.size() - 1}, true};
}
} // namespace angonoka::detail
