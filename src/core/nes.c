#include <stdint.h>

#include "nes.h"

static uint8_t read_byte(struct zc_nes *nes, uint16_t addr) {
    return nes->cpu.ram[addr];
}

static uint8_t read_next_byte(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.pc);
    ++nes->cpu.pc;
    return data;
}

static void abu(struct zc_nes *nes) {
    uint8_t low = read_next_byte(nes);
    uint8_t high = read_next_byte(nes);
    nes->cpu.ea = low | high << 8;
}

static void imm(struct zc_nes *nes) {
    nes->cpu.ea = nes->cpu.pc;
    ++nes->cpu.pc;
}

static void zpg(struct zc_nes *nes) {
    nes->cpu.ea = read_next_byte(nes);
}

static void lda(struct zc_nes *nes) {
    nes->cpu.a = read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

void cpu_step(struct zc_nes *nes) {
    uint8_t opc = read_next_byte(nes);
    if (opc == 0xA5) {
        zpg(nes);
        lda(nes);
    } else if (opc == 0xA9) {
        imm(nes);
        lda(nes);
    } else if (opc == 0xAD) {
        abu(nes);
        lda(nes);
    }
}

void nes_step(struct zc_nes *nes) {
    cpu_step(nes);
}
