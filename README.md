# zcnes

## Build

```bash
$ git submodule update --init
$ cmake --preset dev
$ cmake --build build
$ ctest --test-dir build # -T Test -T Coverage
```