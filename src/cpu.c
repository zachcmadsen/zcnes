#include <stdint.h>

#include "cpu.h"

static uint8_t cpu_read(struct cpu *cpu, uint16_t addr) {
    return cpu->ram[addr];
}

static void imm(struct cpu *cpu) {
    cpu->ea = cpu->pc;
    ++cpu->pc;
}

static void lda(struct cpu *cpu) {
    cpu->a = cpu_read(cpu, cpu->ea);
    cpu->z = cpu->a == 0;
    cpu->n = cpu->a & 0x80;
}

void cpu_step(struct cpu *cpu) {
    uint8_t opc = cpu_read(cpu, cpu->pc);
    ++cpu->pc;

    imm(cpu);
    lda(cpu);
}
