#include <fmt/printf.h>
#include <clipp.h>
#include <string>
#include <memory>

namespace {

struct Options {
	std::string filename;
};
}



int main(int argc, char** argv) {
	using namespace clipp;

	Options options;

	group cli{
	    value("input file", options.filename)
	};

	if(!parse(argc, argv, cli)) {
		fmt::print("{}", make_man_page(cli, argv[0]));
		return 1;
	}

	return 0;
}
