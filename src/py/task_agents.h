#pragma once

namespace angonoka::stun {
class TaskAgents {
public:
    TaskAgents() = default;
    TaskAgents(const std::vector<veci>& data);

    decltype(auto) operator[](gsl::index i) const noexcept;

private:
    std::unique_ptr<Int[]> int_data;
    std::unique_ptr<viewi[]> spans;
    viewv task_agents;
};
} // namespace angonoka::stun
