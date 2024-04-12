#include <stdarg.h>
#include <stdio.h>

#include "log.h"

void zc_log(enum zc_log_level level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    switch (level) {
    case zc_log_error:
        fprintf(stderr, "error: ");
        break;
    default:
        break;
    }

    // TODO: Handle vfprintf failures?
    vfprintf(stderr, fmt, args);
    va_end(args);
}
