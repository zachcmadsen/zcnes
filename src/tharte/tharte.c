#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#include "cpu.h"
#include "log.h"
#include "tharte.h"

struct ram_state {
    uint16_t addr;
    uint8_t data;
};

struct cpu_state {
    uint16_t pc;
    uint8_t s, a, x, y, p;
    struct ram_state ram[20];
    size_t ram_size;
};

/// Reads bytes from file `filename` into `buf`, writing the number of bytes
/// read to `size`.
static int read_file(const char *filename, uint8_t **buf, size_t *size) {
    int rc = 0;
    uint8_t *file_buf = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        zc_log(zc_log_error, "could not open file '%s'\n", filename);
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

    // TODO: How to test malloc failing?
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

/// Parses `jv` into an unsigned 64-bit integer `n`.
///
/// This is a helper function for more specific parse functions, e.g.,
/// `parse_uint8_t`.
static int parse_number(struct json_value_s *jv, uint64_t *n) {
    struct json_number_s *jn = json_value_as_number(jv);
    if (!jn) {
        return -1;
    }

    errno = 0;
    char *end = NULL;
    unsigned long ul = strtoul(jn->number, &end, 10);
    if (jn->number == end || *end != '\0' || errno) {
        return -1;
    }

    *n = ul;

    return 0;
}

/// Parses `jv` into an unsigned 8-bit integer `n`.
static int parse_uint8_t(struct json_value_s *jv, uint8_t *u) {
    uint64_t n;
    if (parse_number(jv, &n)) {
        return -1;
    }

    if (n > UINT8_MAX) {
        return -1;
    }

    *u = (uint8_t)n;

    return 0;
}

/// Parses `jv` into an unsigned 16-bit integer `n`.
static int parse_uint16_t(struct json_value_s *jv, uint16_t *u) {
    uint64_t n;
    if (parse_number(jv, &n)) {
        return -1;
    }

    if (n > UINT16_MAX) {
        return -1;
    }

    *u = (uint16_t)n;

    return 0;
}

int parse_ram_state(struct json_value_s *jv, struct ram_state *state) {
    struct json_array_s *ja = json_value_as_array(jv);
    if (!ja) {
        return -1;
    }

    if (ja->length != 2) {
        return -1;
    }

    struct json_array_element_s *jae = ja->start;
    if (parse_uint16_t(jae->value, &state->addr)) {
        return -1;
    }

    if (parse_uint8_t(jae->next->value, &state->data)) {
        return -1;
    }

    return 0;
}

int parse_ram(const struct json_object_element_s *joe,
              struct cpu_state *state) {
    struct json_array_s *ja = json_value_as_array(joe->value);
    if (!ja) {
        return -1;
    }

    int i = 0;
    for (struct json_array_element_s *jae = ja->start; jae;
         jae = jae->next, ++i) {
        if (parse_ram_state(jae->value, &state->ram[i])) {
            return -1;
        }
    }
    state->ram_size = i;

    return 0;
}

static int parse_cpu_state(struct json_value_s *jv, struct cpu_state *state) {
    struct json_object_s *jo = json_value_as_object(jv);
    if (!jo) {
        return -1;
    }

    for (struct json_object_element_s *joe = jo->start; joe; joe = joe->next) {
        const char *name = joe->name->string;

        if (strcmp(name, "pc") == 0) {
            if (parse_uint16_t(joe->value, &state->pc)) {
                return -1;
            }
        } else if (strcmp(name, "s") == 0) {
            if (parse_uint8_t(joe->value, &state->s)) {
                return -1;
            }
        } else if (strcmp(name, "a") == 0) {
            if (parse_uint8_t(joe->value, &state->a)) {
                return -1;
            }
        } else if (strcmp(name, "x") == 0) {
            if (parse_uint8_t(joe->value, &state->x)) {
                return -1;
            }
        } else if (strcmp(name, "y") == 0) {
            if (parse_uint8_t(joe->value, &state->y)) {
                return -1;
            }
        } else if (strcmp(name, "p") == 0) {
            if (parse_uint8_t(joe->value, &state->p)) {
                return -1;
            }
        } else if (strcmp(name, "ram") == 0) {
            if (parse_ram(joe, state)) {
                return -1;
            }
        }
    }

    return 0;
}

static int parse_test(struct json_value_s *jv, const char **name,
                      struct cpu_state *init, struct cpu_state *final) {
    struct json_object_s *jo = json_value_as_object(jv);
    if (!jo) {
        return -1;
    }

    for (struct json_object_element_s *joe = jo->start; joe; joe = joe->next) {
        const char *joe_name = joe->name->string;

        if (strcmp(joe_name, "name") == 0) {
            struct json_string_s *js = json_value_as_string(joe->value);
            if (!js) {
                return -1;
            }

            *name = js->string;
        } else if (strcmp(joe_name, "initial") == 0) {
            if (parse_cpu_state(joe->value, init)) {
                return -1;
            }
        } else if (strcmp(joe_name, "final") == 0) {
            if (parse_cpu_state(joe->value, final)) {
                return -1;
            }
        }
    }

    return 0;
}

int tharte_run(const char *filename) {
    int rc = 0;
    struct json_value_s *jv = NULL;

    uint8_t *buf = NULL;
    size_t size = 0;
    if (read_file(filename, &buf, &size)) {
        rc = -1;
        goto cleanup;
    }

    jv = json_parse(buf, size);
    if (!jv) {
        zc_log(zc_log_error, "invalid JSON\n");
        rc = -1;
        goto cleanup;
    }

    struct json_array_s *ja = json_value_as_array(jv);
    if (!ja) {
        rc = -1;
        goto cleanup;
    }

    struct cpu *cpu = malloc(sizeof(struct cpu));

    for (struct json_array_element_s *jae = ja->start; jae; jae = jae->next) {
        const char *name;
        struct cpu_state init, final;
        if (parse_test(jae->value, &name, &init, &final)) {
            rc = -1;
            goto cleanup;
        }

        memset(cpu->ram, 0, 0x10000);

        cpu->pc = init.pc;
        cpu->s = init.s;
        cpu->a = init.a;
        cpu->x = init.x;
        cpu->y = init.y;
        for (size_t i = 0; i < init.ram_size; ++i) {
            struct ram_state ram_state = init.ram[i];
            cpu->ram[ram_state.addr] = ram_state.data;
        }

        cpu_step(cpu);

        bool passed = true;
        passed &= cpu->pc == final.pc;
        passed &= cpu->s == final.s;
        passed &= cpu->a == final.a;
        passed &= cpu->x == final.x;
        passed &= cpu->y == final.y;
        for (size_t i = 0; i < init.ram_size; ++i) {
            struct ram_state ram_state = final.ram[i];
            passed &= cpu->ram[ram_state.addr] == ram_state.data;
        }

        if (!passed) {
            printf("name: %s\n", name);
            // TODO: Print out mismatched values.
        }
    }

cleanup:
    if (jv) {
        free(jv);
    }

    return rc;
}
