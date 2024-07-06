# zcnes

A work in progress cycle-accurate NES emulator.

## Build

You need CMake to build `zcnes`. Build and run the tests with

```bash
$ cmake -S . -B build
$ cmake --build build
$ ctest --test-dir build
```