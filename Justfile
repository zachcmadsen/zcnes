build:
    cmake --build build

configure: clean
    cmake --preset dev

test: build
    ctest --test-dir build

clean:
    rm -rf build