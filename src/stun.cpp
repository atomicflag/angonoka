#include <clipp.h>
#include <fmt/printf.h>
#include <memory>
#include <string>

#include "stun_dag/common.h"
#include "stun_dag/schedule_info.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <utility>

namespace angonoka::stun_dag {

} // namespace angonoka::stun_dag

namespace {
struct Options {
};
} // namespace

int main(int /*unused*/, char** /*unused*/)
{
    using namespace clipp;
    // TODO: Add a test STUN invocation.

    // Options options;

    // group cli{value("input file", options.filename)};
    //
    // if (!parse(argc, argv, cli)) {
    //     fmt::print("{}", make_man_page(cli, *argv));
    //     return 1;
    // }

    return 0;
}
