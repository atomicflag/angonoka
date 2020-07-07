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
-march=x86-64 \
-mtune=intel \
-fomit-frame-pointer \
-ffunction-sections \
-fdata-sections \
-falign-functions=32 \
-mllvm -polly -mllvm -polly-vectorizer=stripmine \
-fno-stack-protector
endef
RELEASE_LDFLAGS := -Wl,--gc-sections
LLVM_ROOT := $(shell readlink -m $$(which clang-tidy)/../..)
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
	sed -i -e 's/\([^a-zA-Z]-\)I/\1isystem/g' **.pc
	meson $(MESON_ARGS) ..

.PHONY: test
test:
	export LLVM_PROFILE_FILE=angonoka.profraw
	build/angonoka_test

.PHONY: ninja
ninja: build/build.ninja
	$(BUILD_ENV)
	ninja

.PHONY: debug
debug: MESON_ARGS=--buildtype debug \
	-Db_sanitize=address,undefined \
	-Db_lundef=false
debug: ninja

.PHONY: install
install: release
	$(BUILD_ENV)
	ninja install
	cd ..
	LIBS=$$(ldd build/angonoka-x86_64 | sed -n \
		'/\(\.conan\|\/usr\/local\|\/opt\/llvm\)/s/.*=> \(.*\) (.*/\1/gp' \
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

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: build-cov
build-cov: MESON_ARGS=--buildtype debug
build-cov: CXXFLAGS=-fprofile-instr-generate -fcoverage-mapping
build-cov: ninja

.PHONY: check-cov
check-cov: build-cov
	llvm-profdata merge -sparse angonoka.profraw -o angonoka.profdata
	llvm-cov report \
		build/angonoka_test \
		-instr-profile=angonoka.profdata \
		src

.PHONY: format
format:
	$(BUILD_ENV)
	ninja clang-format

.PHONY: check-format
check-format:
	echo Running clang-format
	! clang-format -output-replacements-xml \
		$$(find src test -name '*.h' -o -name '*.cpp') \
	| grep -q '<replacement '

.PHONY: check-tidy
check-tidy:
	echo Running clang-tidy
	cd build
	sed -i \
		-e 's/-fsanitize=[a-z,]*//g' \
		-e 's/-pipe//g' \
		-e 's/-fno-omit-frame-pointer//g' \
		-e 's/--coverage//g' \
		compile_commands.json
	python3 <<EOF
		import json
		data = json.load(open('compile_commands.json'))
		def keep(f): return '@' not in f['file'] and \
		    'angonoka_test@exe' not in f['output']
		data = tuple(filter(keep, data))
		json.dump(data, open('compile_commands.json', 'w'))
	EOF
	! python3 $(LLVM_ROOT)/share/clang/run-clang-tidy.py \
		$$(echo | clang -v -E -x c++ - 2>&1 | \
			sed -n 's/^ \(\/[^ ]*\)/-extra-arg=-isystem\1/p' | \
			tr '\n' ' ') \
		-quiet 2>/dev/null | \
		grep -E '(note:|error:|warning:)'	

.PHONY: check
check: check-format check-tidy

.PHONY: clean
clean:
	rm -rf build
