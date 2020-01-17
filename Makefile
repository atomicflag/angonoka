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

build/conaninfo.txt:
	@mkdir -p build && \
	cd build && \
	export CXXFLAGS="$$CXXFLAGS $(RELEASE_CXXFLAGS)" && \
	export CFLAGS="$$CFLAGS $(RELEASE_CXXFLAGS)" && \
	export LDFLAGS="$$LDFLAGS $(RELEASE_LDFLAGS)" && \
	conan install -b missing ..

build/build.ninja: build/conaninfo.txt
	@cd build && \
	export PKG_CONFIG_PATH=$$(pwd) && \
	export BOOST_ROOT=$$(pkg-config --variable=prefix boost) && \
	export CXXFLAGS="$$CXXFLAGS $(CXXFLAGS)" && \
	export CFLAGS="$$CFLAGS $(CXXFLAGS)" && \
	export LDFLAGS="$$LDFLAGS $(LDFLAGS)" && \
	sed -i -e 's/\([^a-zA-Z]-\)I/\1isystem/g' **.pc; \
	meson $(MESON_ARGS) ..

.PHONY: test
test:
	@build/angonoka_test

.PHONY: ninja
ninja: build/build.ninja
	@cd build && ninja

.PHONY: debug
debug: MESON_ARGS=--buildtype debug \
	-Db_sanitize=address,undefined \
	-Db_lundef=false
debug: ninja

.PHONY: install
install: release
	@cd build && ninja install && cd .. && \
	LIBS=$$(ldd build/angonoka-x86_64 | sed -n \
		'/\(\.conan\|\/usr\/local\|\/opt\/llvm\)/s/.*=> \(.*\) (.*/\1/gp' \
	) && \
	mkdir -p dist/lib64 && \
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
build-cov: MESON_ARGS=--buildtype release \
	-Db_ndebug=true \
	-Db_coverage=true
build-cov: ninja

.PHONY: check-cov
check-cov: build-cov
	@cd build && gcovr --gcov-executable 'llvm-cov gcov' -f ../src -r ../

.PHONY: format
format:
	@cd build && ninja clang-format

.PHONY: check-format
check-format:
	@echo Running clang-format; \
	! clang-format -output-replacements-xml \
		$$(find src test -name '*.h' -o -name '*.cpp') \
	| grep -q '<replacement '

.PHONY: check-tidy
check-tidy:
	@echo Running clang-tidy; \
	cd build && \
	sed -i \
		-e 's/-fsanitize=[a-z,]*//g' \
		-e 's/-pipe//g' \
		-e 's/-fno-omit-frame-pointer//g' \
		-e 's/--coverage//g' \
		compile_commands.json && \
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
	@rm -rf build
