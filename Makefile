#
# This Makefile assumes you have a full LLVM toolchain including
# clang and polly. A proper conanfile.py is WIP.
#

.ONESHELL:
.SILENT:
.DEFAULT_GOAL := debug
CLANG_TIDY_FILES ?= ../src
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
-fno-stack-protector \
-fno-semantic-interposition \
-fPIC
endef
RELEASE_LDFLAGS := -Wl,--gc-sections,-Bsymbolic-functions
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
	conan install -l ../conan.lock ..

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
			.replace(' -o ', ' $(CLANG_BUILTIN) '
			'-Dgsl_CONFIG_CONTRACT_CHECKING_OFF '
			'-Dgsl_CONFIG_UNENFORCED_CONTRACTS_ELIDE '
			'-DNDEBUG '
			'-o ', 1) \
			.replace('-DUNIT_TEST', '')
		json.dump(data, open('compile_commands.json', 'w'))
	EOF

.PHONY: test
test:
	export OMP_NUM_THREADS=1
	for t in $$(find build -name '*_test'); do
		test_name=$$(basename $$t)
		echo "$$test_name:"
		LLVM_PROFILE_FILE=$$t.profraw $$t -r compact
	done

.PHONY: test/functional
test/functional:
	cd test/functional
	pytest -qx suite.py

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
	mkdir -p debug
	cd dist
	for f in $$(find bin lib64 -type f); do
		llvm-objcopy --only-keep-debug $$f $$f.dbg
		DIR=../debug/$$(dirname $$f)
		mkdir -p $$DIR
		mv $$f.dbg $$DIR
		llvm-strip --strip-unneeded $$f
	done

.PHONY: release
release: MESON_ARGS=--prefix \
	$$(readlink -m ../dist) \
	--buildtype release \
	-Db_lto=true \
	-Db_ndebug=true \
	-Dstrip=false
release: CXXFLAGS=$(RELEASE_CXXFLAGS)
release: LDFLAGS=$(RELEASE_LDFLAGS)
release: ninja

.PHONY: plain
plain: MESON_ARGS=--buildtype plain
plain: ninja

.PHONY: build/cov
build/cov: MESON_ARGS=--buildtype debugoptimized -Dtests=enabled -Dopenmp=disabled
build/cov: CXXFLAGS=-fprofile-instr-generate -fcoverage-mapping -DANGONOKA_COVERAGE
build/cov: ninja

.PHONY: check/cov
check/cov: build/cov
	llvm-profdata merge \
		-sparse \
		$$(find . -name '*.profraw') \
		-o angonoka.profdata
	llvm-cov report \
		build/src/angonoka-x86_64 \
		-instr-profile=angonoka.profdata \
		--ignore-filename-regex='.*\.a\.p'

.PHONY: check/show
check/show: check/cov
	llvm-cov show \
		build/src/angonoka-x86_64 \
		-instr-profile=angonoka.profdata \
		-format=html -o html \
		--ignore-filename-regex='.*\.a\.p'

.PHONY: format
format: build/build.ninja
	$(BUILD_ENV)
	ninja clang-format

.PHONY: check/format
check/format:
	echo Running clang-format
	clang-format --Werror -n \
		$$(find src test \
			-name '*.h' -o -name '*.cpp')

.PHONY: check/tidy
check/tidy: build/build.ninja
	echo Running clang-tidy
	test -z "$(CLANG_TIDY_FILES)" && { echo No changes; exit 0; }
	cd build
	[ -e compile_commands.json.bak ] && \
		mv compile_commands.json.bak \
			compile_commands.json
	cp compile_commands.json compile_commands.json.bak
	sed -i \
		-e 's/ -fsanitize=[a-z,]*//g' \
		-e 's/ -pipe//g' \
		-e 's/ -fno-omit-frame-pointer//g' \
		-e 's/ --coverage//g' \
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
	run-clang-tidy -quiet $(CLANG_TIDY_FILES) 2>/dev/null
	EXIT_CODE=$$?
	mv compile_commands.json.bak compile_commands.json
	exit $$EXIT_CODE

.PHONY: check
check: check/format check/tidy

.PHONY: clean
clean:
	git clean -fxd

tools/visualizer/node_modules:
	cd tools/visualizer
	yarn install

tools/visualizer/dist: tools/visualizer/node_modules
	cd tools/visualizer
	yarn build

.PHONY: check-js
check-js: tools/visualizer/node_modules
	cd tools/visualizer
	yarn lint

.PHONY: test-js
test-js: tools/visualizer/node_modules
	cd tools/visualizer
	yarn test

.PHONY: format-js
format-js: tools/visualizer/node_modules
	cd tools/visualizer
	yarn format

.PHONY: install-deps
install-deps:
	mkdir -p deps
	cd deps
	export CXXFLAGS="$$CXXFLAGS $(RELEASE_CXXFLAGS)"
	export CFLAGS="$$CFLAGS $(RELEASE_CXXFLAGS)"
	export LDFLAGS="$$LDFLAGS $(RELEASE_LDFLAGS)"
	conan install -u -b missing ..

.PHONY: update-deps
update-deps:
	docker run --rm -i \
	  --name temp \
	  -e CONAN_TOKEN \
	  -e CONAN_USER \
	  -v $$(pwd):/app \
	  --entrypoint /bin/bash \
	  registry.gitlab.com/signal9/cpp-env:13.0.0 <<"EOF"
	python3 -m pip --no-cache-dir install --upgrade conan
	apt-get update
	apt-get install -y --no-install-recommends jq
	conan remote add signal9 https://signal9.jfrog.io/artifactory/api/conan/conan --insert
	conan config set general.revisions_enabled=1
	conan user -p $$CONAN_TOKEN -r signal9 $$CONAN_USER
	cd /app
	make install-deps
	cp deps/conan.lock conan.lock
	conan info -l conan.lock -u -j deps.json .
	deps=$$(jq -r '.[] | select(.is_ref) | "\(.reference)#\(.revision):\(.id)"' deps.json)
	for d in $$deps; do conan upload -r signal9 --parallel $$d; done
	rm -rf deps deps.json
	EOF
