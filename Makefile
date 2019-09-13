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

.PHONY: ninja
ninja: build/build.ninja
	@cd build && ninja

.PHONY: debug
debug: MESON_ARGS=--buildtype debug -Db_sanitize=address,undefined -Db_lundef=false
debug: ninja

.PHONY: install
install: release
	@cd build && ninja install && cd .. && \
	LIBS=$$(ldd build/angonoka-x86_64 | sed -n '/\(\.conan\|\/usr\/local\|\/opt\/llvm\)/s/.*=> \(.*\) (.*/\1/gp') && \
	mkdir -p dist/lib64 && \
	cp $$LIBS dist/lib64 && \
	patchelf --set-rpath '$$ORIGIN/../lib64' dist/bin/angonoka-x86_64

.PHONY: release
release: MESON_ARGS=--prefix $$(readlink -m ../dist) --buildtype release -Db_lto=true -Db_ndebug=true -Dstrip=true
release: CXXFLAGS=$(RELEASE_CXXFLAGS)
release: LDFLAGS=$(RELEASE_LDFLAGS)
release: ninja

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: format
format:
	@clang-format -i $$(find src test -name '*.cpp' -o -name '*.h')

.PHONY: check-format
check-format:
	@! clang-format -output-replacements-xml $$(find src test -name '*.h' -o -name '*.cpp') | grep -q '<replacement '

.PHONY: check-tidy
check-tidy:
	@cd build && ! python3 $(LLVM_ROOT)/share/clang/run-clang-tidy.py -extra-arg=-isystem$(LLVM_ROOT)/include/c++/v1/ -header-filter=../src | grep -E '(note:|warning:)'

.PHONY: check
check: check-format check-tidy

.PHONY: clean
clean:
	@rm -rf build
