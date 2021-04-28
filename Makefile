#
# This Makefile assumes you have a full LLVM toolchain including
# clang and polly. A proper conanfile.py is WIP.
#

.ONESHELL:
.SILENT:
.DEFAULT_GOAL := debug
define RELEASE_CXXFLAGS =
-pipe \
-O3 \
-march=haswell \
-mtune=haswell \
-fomit-frame-pointer \
-ffunction-sections \
-fdata-sections \
-falign-functions=32 \
-mllvm -polly -mllvm -polly-vectorizer=stripmine \
-fno-stack-protector
endef
RELEASE_LDFLAGS := -Wl,--gc-sections
LLVM_ROOT := $(shell readlink -m $$(which clang-tidy)/../..)
CLANG_BUILTIN := $(shell echo | clang -v -E -x c++ - 2>&1 \
	| sed -nE 's@^ (/[^ ]*)@-isystem\1@p' | tr '\n' ' ')
define BUILD_ENV
cd build
. ./activate.sh
endef

build/conaninfo.txt:
	mkdir -p build
	cd build
	export CXXFLAGS="$$CXXFLAGS $(RELEASE_CXXFLAGS)"
	export CFLAGS="$$CFLAGS $(RELEASE_CXXFLAGS)"
	export LDFLAGS="$$LDFLAGS $(RELEASE_LDFLAGS)"
	conan install -b missing ..

build/build.ninja: build/conaninfo.txt
	$(BUILD_ENV)
	export PKG_CONFIG_PATH=$$(pwd)
	export CXXFLAGS="$$CXXFLAGS $(CXXFLAGS)"
	export CFLAGS="$$CFLAGS $(CXXFLAGS)"
	export LDFLAGS="$$LDFLAGS $(LDFLAGS)"
	sed -Ei 's/([^a-zA-Z]-)I/\1isystem/g' **.pc
	meson $(MESON_ARGS) ..
	python3 <<EOF
		import json
		data = json.load(open('compile_commands.json'))
		for f in data: f['command'] = f['command'] \
			.replace(' -isystem', ' $(CLANG_BUILTIN) '
			'-Dgsl_CONFIG_CONTRACT_CHECKING_OFF '
			'-Dgsl_CONFIG_UNENFORCED_CONTRACTS_ASSUME '
			'-DNDEBUG '
			'-isystem', 1) \
			.replace('-DUNIT_TEST', '')
		json.dump(data, open('compile_commands.json', 'w'))
	EOF

.PHONY: test
test:
	for t in $$(find build -name '*_test'); do
		test_name=$$(basename $$t)
		printf "$$test_name:\n  "
		LLVM_PROFILE_FILE=$$t.profraw $$t
	done

# .PHONY: benchmark
# benchmark:
# 	build/benchmark/angonoka_benchmark

.PHONY: ninja
ninja: build/build.ninja
	$(BUILD_ENV)
	ninja

.PHONY: debug
debug: MESON_ARGS=--buildtype debug \
	-Db_sanitize=address,undefined \
	-Db_lundef=false \
	-Dtests=enabled
debug: ninja

.PHONY: install
install: release
	$(BUILD_ENV)
	ninja install
	cd ..
	LIBS=$$(ldd build/src/angonoka-x86_64 | sed -nE \
		'\@(\.conan|/usr/local|/opt/llvm)@s/.*=> (.*) \(.*/\1/gp' \
	)
	mkdir -p dist/lib64
	cp $$LIBS dist/lib64

.PHONY: release
release: MESON_ARGS=--prefix \
	$$(readlink -m ../dist) \
	--buildtype release \
	-Db_lto=true \
	-Db_ndebug=true \
	-Dstrip=true
release: CXXFLAGS=$(RELEASE_CXXFLAGS)
release: LDFLAGS=$(RELEASE_LDFLAGS)
release: ninja

# .PHONY: release/benchmark
# release/benchmark: MESON_ARGS=--buildtype release \
# 	-Db_lto=true \
# 	-Db_ndebug=true \
# 	-Dbenchmark=enabled
# release/benchmark: CXXFLAGS=$(RELEASE_CXXFLAGS)
# release/benchmark: LDFLAGS=$(RELEASE_LDFLAGS)
# release/benchmark: ninja

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: build/cov
build/cov: MESON_ARGS=--buildtype debug -Dtests=enabled
build/cov: CXXFLAGS=-fprofile-instr-generate -fcoverage-mapping -DANGONOKA_COVERAGE
build/cov: ninja

.PHONY: check/cov
check/cov: build/cov
	llvm-profdata merge \
		-sparse \
		$$(find . -name '*.profraw') \
		-o angonoka.profdata
	llvm-cov report \
		build/test/angonoka_test \
		-instr-profile=angonoka.profdata \
		src

.PHONY: format
format:
	$(BUILD_ENV)
	ninja clang-format

.PHONY: check/format
check/format:
	echo Running clang-format
	clang-format --Werror -n \
		$$(find src test \
			-name '*.h' -o -name '*.cpp')

.PHONY: check/tidy
check/tidy:
	echo Running clang-tidy
	cd build
	[ -e compile_commands.json.bak ] && \
		mv compile_commands.json.bak \
			compile_commands.json
	cp compile_commands.json compile_commands.json.bak
	sed -i \
		-e 's/-fsanitize=[a-z,]*//g' \
		-e 's/-pipe//g' \
		-e 's/-fno-omit-frame-pointer//g' \
		-e 's/--coverage//g' \
		compile_commands.json
	python3 <<EOF
		import json
		data = json.load(open('compile_commands.json'))
		def keep(f): return 'meson-generated' \
			not in f['output'] and \
			not f['output'].startswith('test')
		data = tuple(filter(keep, data))
		json.dump(data, open('compile_commands.json', 'w'))
	EOF
	python3 $(LLVM_ROOT)/share/clang/run-clang-tidy.py \
		-quiet $$(find ../src \
			\( -name '*.cpp' -o -name '*.h' \) \
			! -wholename '../src/stun.cpp' \
			) 2>/dev/null
	EXIT_CODE=$$?
	mv compile_commands.json.bak compile_commands.json
	exit $$EXIT_CODE

.PHONY: check
check: check/format check/tidy

.PHONY: clean
clean:
	rm -rf build
