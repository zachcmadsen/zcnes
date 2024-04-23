#pragma once

#include <stddef.h>
#include <stdint.h>

/// Reads the contents of file `filename` into `buf` and stores the size of
/// `buf` in `size`.
int read_file(const char *filename, uint8_t **buf, size_t *size);
