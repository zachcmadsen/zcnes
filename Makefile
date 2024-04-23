.PHONY: default
default: build

# TODO: Add an error message when cmake/ninja aren't installed. See
# https://github.com/mattgodbolt/xania/blob/main/Makefile for an example.
build/build.ninja:
	cmake --preset dev

.PHONY: build
build: build/build.ninja
	cmake --build build

.PHONY: test
test: build
	ctest --test-dir build

.PHONY: clean
clean:
	rm -rf build
