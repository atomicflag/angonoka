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
-fno-stack-protector
endef
RELEASE_LDFLAGS := -Wl,--gc-sections

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
	@cd build && ninja install

.PHONY: release
release: MESON_ARGS=--prefix $$(readlink -m ../dist) --buildtype release -Db_lto=true -Db_ndebug=true -Dstrip=true
release: CXXFLAGS=$(RELEASE_CXXFLAGS)
release: LDFLAGS=$(RELEASE_LDFLAGS)
release: ninja

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: clean
clean:
	@rm -rf build
