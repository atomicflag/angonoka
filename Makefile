.DEFAULT_GOAL := debug

build/conaninfo.txt:
	@mkdir -p build && \
	cd build && \
	conan install -b missing ..

build/build.ninja: build/conaninfo.txt
	@cd build && \
	export PKG_CONFIG_PATH=$$(pwd) && \
	export BOOST_ROOT=$$(pkg-config --variable=prefix boost) && \
	sed -i -e 's/\([^a-zA-Z]-\)I/\1isystem/g' **.pc; \
	meson $(MESON_ARGS) ..

.PHONY: ninja
ninja: build/build.ninja
	@cd build && ninja

.PHONY: debug
debug: MESON_ARGS=--buildtype debug -Db_sanitize=address,undefined -Db_lundef=false
debug: ninja

.PHONY: release
release: MESON_ARGS=--buildtype release -Db_lto=true -Db_ndebug=true
release: ninja

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: clean
clean:
	@rm -rf build
