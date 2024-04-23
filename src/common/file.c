#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int read_file(const char *filename, uint8_t **buf, size_t *size) {
    int rc = 0;
    uint8_t *file_buf = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        rc = -1;
        goto error;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        rc = -1;
        goto error;
    }

    // ftell returns the number of bytes from the beginning of the file to the
    // stream cursor if the stream is opened in binary mode.
    long file_size = ftell(fp);
    if (file_size == -1L) {
        rc = -1;
        goto error;
    }

    // Go back to the beginning before reading.
    if (fseek(fp, 0, SEEK_SET) != 0) {
        rc = -1;
        goto error;
    }

    file_buf = malloc(file_size);
    if (!file_buf) {
        rc = -1;
        goto error;
    }

    if (fread(file_buf, 1, file_size, fp) != (unsigned long)file_size) {
        rc = -1;
        goto error;
    }

    *buf = file_buf;
    *size = file_size;

cleanup:
    if (fp) {
        fclose(fp);
    }

    return rc;

error:
    if (file_buf) {
        free(file_buf);
    }

    goto cleanup;
}
