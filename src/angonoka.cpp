#include <clipp.h>
#include <fmt/printf.h>
#include <memory>
#include <string>

namespace {
struct Options {
	std::string filename;
};
} // namespace

int main(int argc, char** argv)
{
	using namespace clipp;

	Options options;

	group cli {value("input file", options.filename)};

	if (!parse(argc, argv, cli)) {
		fmt::print("{}", make_man_page(cli, *argv));
		return 1;
	}

	return 0;
}
