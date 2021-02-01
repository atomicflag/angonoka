#include "load.h"
#include "../common.h"
#include "../exceptions.h"
#include "validation.h"
#include <range/v3/algorithm/find.hpp>

namespace {
using angonoka::validation::result;

/**
    Recursive helper-function for task list validation.

    Parses blocks such as these:

      - name: task 1
        duration: 1h
        subtasks:
          - name: task 2
            duration: 2h
            subtasks:
              - name: task 3
                duration: 3h

    @param node     YAML node containing the task list
    @param scope    Name of the validation context

    @return Success status or an error string
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
        optional("subtasks", validate_task_list)
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
        required("tasks", validate_task_list)
    );
    // clang-format on
    if (const auto r = schema(node); !r)
        throw angonoka::SchemaError{r.error()};
}

angonoka::System parse_yaml(const YAML::Node& root)
{
    using namespace angonoka;

    validate_configuration(root);
    System system;
    detail::parse_agents(root["agents"], system);
    detail::parse_tasks(root["tasks"], system);

    Ensures(!system.tasks.empty());
    Ensures(!system.agents.empty());
    return system;
}
} // namespace

namespace angonoka {
System load_text(gsl::czstring text)
{
    Expects(text != nullptr);

    return parse_yaml(YAML::Load(text));
}

System load_file(std::string_view path)
{
    Expects(!path.empty());

    return parse_yaml(YAML::LoadFile(std::string{path}));
}
} // namespace angonoka

namespace angonoka::detail {
std::pair<GroupIndex, bool>
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
