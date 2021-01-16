#include "load.h"
#include "../common.h"
#include "../exceptions.h"
#include "validation.h"
#include <range/v3/algorithm/find.hpp>

namespace {
using angonoka::validation::result;

/**
    TODO: doc
*/
result
validate_task_list(const YAML::Node& node, std::string_view scope)
{
    using namespace angonoka::validation;
    // clang-format off
    return sequence(attributes(
        optional("group"),
        "name",
        optional("id"),
        optional("depends_on", any_of(
            scalar(),
            sequence()
        )),
        required("duration", any_of(
            attributes(
                "min",
                "max"
            ),
            scalar()
        )),
        optional("subtasks", 
            []<typename... T>(T&&... args) {
                return validate_task_list(std::forward<T>(args)...);
            }
        )
    ))(node, scope);
    // clang-format on
}
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
                optional("performance", any_of(
                    attributes(
                        "min",
                        "max"
                    ),
                    scalar()
                )),
                optional("groups", sequence())
            ))
        ),
        required("tasks",
            any_of(
                // TODO: Legacy
                values(attributes(
                    optional("group"),
                    required("duration", any_of(
                        attributes(
                            "min",
                            "max"
                        ),
                        scalar()
                    ))
                )),
                // New
                validate_task_list
            )
        )
    );
    // clang-format on
    if (const auto r = schema(node); !r)
        throw angonoka::SchemaError{r.error()};
}
} // namespace

namespace angonoka {
System load_text(gsl::czstring text)
{
    Expects(text != nullptr);

    const auto node = YAML::Load(text);
    validate_configuration(node);
    System system;
    detail::parse_agents(node["agents"], system);
    detail::parse_tasks(node["tasks"], system);

    Ensures(!system.tasks.empty());
    Ensures(!system.agents.empty());
    return system;
}
} // namespace angonoka

namespace angonoka::detail {
std::pair<GroupId, bool>
find_or_insert_group(Groups& groups, std::string_view group)
{
    Expects(!group.empty());

    if (const auto f = ranges::find(groups, group); f != groups.end())
        return {std::distance(groups.begin(), f), false};
    groups.emplace_back(group);

    Ensures(!groups.empty());

    return {groups.size() - 1, true};
}
} // namespace angonoka::detail
