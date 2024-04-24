#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct zc_cpu {
    uint16_t pc;
    uint8_t a, x, y, s;
    bool c, z, i, d, b, v, n;

    bool pg_cross;
    uint16_t ea;

#ifdef ZCNES_PROCESSOR_TESTS
    uint8_t ram[0x10000];
#endif
};

struct zc_cart {
    uint8_t prg_ram[0x2000];
    uint8_t *prg_rom;
    // TODO: Can size fields/variables be uint32_t? Can the size ever be
    // larger?
    size_t prg_rom_size;
};

struct zc_nes {
    struct zc_cpu cpu;
    struct zc_cart cart;

    uint8_t ram[0x800];
};

uint8_t cpu_get_p(struct zc_nes *nes);
void cpu_set_p(struct zc_nes *nes, uint8_t p);

void cpu_reset(struct zc_nes *nes);
// TODO: Only expose this for tests?
void cpu_step(struct zc_nes *nes);

int nes_init(struct zc_nes *nes, const uint8_t *rom, size_t rom_size);
void nes_step(struct zc_nes *nes);
void nes_free(struct zc_nes *nes);
