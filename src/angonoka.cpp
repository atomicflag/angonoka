#include <fmt/printf.h>
#include <yaml-cpp/yaml.h>
#include <clipp.h>
#include <string>
#include <memory>

namespace {

struct Options {
	std::string filename;
};

void foo(const Options& options) {
	const auto yaml = YAML::LoadFile(options.filename);
	fmt::print("{}\n", yaml["var"].Scalar());
}
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

	foo(options);

    return 0;
}
