# zcnes

## Build

Checkout submodules with

```bash
$ git submodule update --init
```

You need CMake to build `zcnes`. Build and run the tests with

```bash
$ cmake -S . -B build
$ cmake --build build
$ ctest --test-dir build 
```

Alternatively, if you have `make`, `ninja`, and `gcc`, run

```bash
$ make
$ make test
```

## Coverage

```bash
$ cmake --preset coverage
$ cmake --build build-coverage
$ ctest --test-dir build-coverage
# Capture coverage (-c) from directory (-d) build-coverage and output (-o)
# coverage data to cov.info.
$ lcov -c -d build-coverage -o cov.info
# Generate HTML output for cov.info and output (-o) to out.
$ genhtml cov.info -o out
```