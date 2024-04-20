#include <stdint.h>

#include "nes.h"

static bool overflowing_add(uint8_t a, uint8_t b, uint8_t *out) {
#if defined(__has_builtin) && __has_builtin(__builtin_add_overflow)
    return __builtin_add_overflow(a, b, out);
#else
    *out = a + b;
    return *out < a;
#endif
}

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

static void abx_r(struct zc_nes *nes) {
    uint8_t low = read_next_byte(nes);
    bool page_cross = overflowing_add(low, nes->cpu.x, &low);
    uint8_t high = read_next_byte(nes);
    if (page_cross) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + page_cross) << 8;
}

static void aby_r(struct zc_nes *nes) {
    uint8_t low = read_next_byte(nes);
    bool page_cross = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = read_next_byte(nes);
    if (page_cross) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (high + page_cross) << 8;
}

static void idx(struct zc_nes *nes) {
    uint8_t ptr = read_next_byte(nes);
    read_byte(nes, ptr);
    ptr += nes->cpu.x;
    uint8_t low = read_byte(nes, ptr);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    nes->cpu.ea = low | high << 8;
}

static void imm(struct zc_nes *nes) {
    nes->cpu.ea = nes->cpu.pc;
    ++nes->cpu.pc;
}

static void ind(struct zc_nes *nes) {
    uint8_t ptr_low = read_next_byte(nes);
    uint8_t ptr_high = read_next_byte(nes);
    uint8_t low = read_byte(nes, ptr_low | ptr_high << 8);
    uint8_t high = read_byte(nes, (uint8_t)(ptr_low + 1) | ptr_high << 8);
    nes->cpu.ea = low | high << 8;
}

static void zpg(struct zc_nes *nes) {
    nes->cpu.ea = read_next_byte(nes);
}

static void zpx(struct zc_nes *nes) {
    uint8_t low = read_next_byte(nes);
    read_byte(nes, low);
    nes->cpu.ea = (uint8_t)(low + nes->cpu.x);
}

static void zpy(struct zc_nes *nes) {
    uint8_t low = read_next_byte(nes);
    read_byte(nes, low);
    nes->cpu.ea = (uint8_t)(low + nes->cpu.y);
}

static void lda(struct zc_nes *nes) {
    nes->cpu.a = read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void ldx(struct zc_nes *nes) {
    nes->cpu.x = read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void jmp(struct zc_nes *nes) {
    nes->cpu.pc = nes->cpu.ea;
}

void cpu_step(struct zc_nes *nes) {
    uint8_t opc = read_next_byte(nes);
    if (opc == 0x6C) {
        ind(nes);
        jmp(nes);
    } else if (opc == 0xA1) {
        idx(nes);
        lda(nes);
    } else if (opc == 0xA5) {
        zpg(nes);
        lda(nes);
    } else if (opc == 0xA9) {
        imm(nes);
        lda(nes);
    } else if (opc == 0xAD) {
        abu(nes);
        lda(nes);
    } else if (opc == 0xB5) {
        zpx(nes);
        lda(nes);
    } else if (opc == 0xB6) {
        zpy(nes);
        ldx(nes);
    } else if (opc == 0xB9) {
        aby_r(nes);
        lda(nes);
    } else if (opc == 0xBD) {
        abx_r(nes);
        lda(nes);
    }
}

void nes_step(struct zc_nes *nes) {
    cpu_step(nes);
}
