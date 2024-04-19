#pragma once

#include <stdbool.h>
#include <stdint.h>

struct zc_cpu {
    uint16_t pc;
    uint8_t a, x, y, s;
    bool c, z, i, d, v, n;

    uint16_t ea;

    uint8_t ram[0x10000];
};

struct zc_nes {
    struct zc_cpu cpu;
};

// TODO: Only expose this for tests?
void cpu_step(struct zc_nes *nes);

void nes_step(struct zc_nes *nes);
