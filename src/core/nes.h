#pragma once

#include <stdbool.h>
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

struct zc_nes {
    struct zc_cpu cpu;
};

uint8_t cpu_get_p(struct zc_nes *nes);
void cpu_set_p(struct zc_nes *nes, uint8_t p);

// TODO: Only expose this for tests?
void cpu_step(struct zc_nes *nes);

void nes_step(struct zc_nes *nes);
