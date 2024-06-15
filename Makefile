.PHONY: default
default: build

# TODO: Add an error message when cmake/ninja aren't installed. See
# https://github.com/mattgodbolt/xania/blob/main/Makefile for an example.
# It could default to Makefiles if ninja isn't installed.
build/build.ninja:
	cmake --preset dev

.PHONY: build
build: build/build.ninja
	cmake --build build

.PHONY: test
test: build
	ctest --test-dir build -j $$(nproc) --output-on-failure

.PHONY: clean
clean:
	rm -rf build
