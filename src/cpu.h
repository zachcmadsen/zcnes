#include <stdbool.h>
#include <stdint.h>

struct cpu {
    uint16_t pc;
    uint8_t a, x, y, s;
    bool c, z, i, d, v, n;

    uint8_t ram[0x10000];

    uint16_t ea;
};

void cpu_step(struct cpu *cpu);
