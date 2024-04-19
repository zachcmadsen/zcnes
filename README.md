# zcnes

## Build

```bash
$ git submodule update --init
$ cmake --preset gcc-clang # or msvc
$ cmake --build build
$ ctest --test-dir build
```