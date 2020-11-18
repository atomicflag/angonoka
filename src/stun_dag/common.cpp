#include "common.h"
#include <fmt/ostream.h>
#include <ostream>

namespace angonoka::stun_dag {
#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const StateItem& item)
{
    fmt::print(os, "{}", item);
    return os;
}
#endif // NDEBUG
} // namespace angonoka::stun_dag
