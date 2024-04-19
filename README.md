# zcnes

## Build

```bash
$ git submodule update --init
$ cmake --preset gcc # or msvc
$ cmake --build build
$ ctest --test-dir build
```