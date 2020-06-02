#include "load.h"
#include "../common.h"
#include "../exceptions.h"
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
            values(attributes(
                optional("perf", attributes(
                    required("min"),
                    required("max")
                )),
                optional("groups", sequence())
            ))
        ),
        required("tasks",
            values(attributes(
                optional("group"),
                required("days", attributes(
                    required("min"),
                    required("max")
                ))
            ))
        )
    );
    // clang-format on
    if (const auto r = schema(node); !r) {
        throw angonoka::InvalidTasksDef{r.error()};
    }
}
} // namespace

namespace angonoka {
System load_text(gsl::czstring text)
{
    Expects(!std::string_view{text}.empty());
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
    Expects(!group.empty());
    if (const auto f = ranges::find(groups, group); f != groups.end())
        return {Int{std::distance(groups.begin(), f)}, false};
    groups.emplace_back(group);
    return {Int{groups.size() - 1}, true};
}
} // namespace angonoka::detail
