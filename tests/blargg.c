#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "log.h"
#include "nes.h"

int run(const char *filename) {
    int rc = 0;
    uint8_t *rom = NULL;
    struct zc_nes *nes = NULL;

    size_t rom_size = 0;
    if (read_file(filename, &rom, &rom_size)) {
        rc = -1;
        goto cleanup;
    }

    nes = malloc(sizeof(struct zc_nes));
    if (nes_init(nes, rom, rom_size)) {
        rc = -1;
        goto cleanup;
    }

    cpu_reset(nes);

    while (nes->cart.prg_ram[0] != 0x80) {
        nes_step(nes);
    }

    while (nes->cart.prg_ram[0] == 0x80) {
        nes_step(nes);
    }

    const char *output = (char *)(nes->cart.prg_ram + 4);
    // TODO: Use strncmp instead?
    if (!strstr(output, "Passed")) {
        rc = -1;
    }

cleanup:
    nes_free(nes);

    if (nes) {

        free(nes);
    }

    if (rom) {
        free(rom);
    }

    return rc;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        zc_log(zc_log_error, "no input file\n");
        return EXIT_FAILURE;
    }

    char *filename = argv[1];

    if (run(filename)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
