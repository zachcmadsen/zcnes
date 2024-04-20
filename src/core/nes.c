#include <stdint.h>

#include "nes.h"

#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow)
#define ZCNES_BUILTINS 1
#endif
#endif

static bool overflowing_add(uint8_t a, uint8_t b, uint8_t *out) {
#ifdef ZCNES_BUILTINS
    return __builtin_add_overflow(a, b, out);
#else
    *out = a + b;
    return *out < a;
#endif
}

static uint8_t read_byte(struct zc_nes *nes, uint16_t addr) {
    return nes->cpu.ram[addr];
}

static uint8_t eat_byte(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.pc);
    ++nes->cpu.pc;
    return data;
}

static void write_byte(struct zc_nes *nes, uint16_t addr, uint8_t data) {
    nes->cpu.ram[addr] = data;
}

static void abu(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    uint8_t high = eat_byte(nes);
    nes->cpu.ea = low | high << 8;
}

static void abx_r(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    bool overflow = overflowing_add(low, nes->cpu.x, &low);
    uint8_t high = eat_byte(nes);
    if (overflow) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void abx_w(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    bool overflow = overflowing_add(low, nes->cpu.x, &low);
    uint8_t high = eat_byte(nes);
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void aby_r(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    bool overflow = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = eat_byte(nes);
    if (overflow) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void aby_w(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    bool overflow = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = eat_byte(nes);
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void idx(struct zc_nes *nes) {
    uint8_t ptr = eat_byte(nes);
    read_byte(nes, ptr);
    ptr += nes->cpu.x;
    uint8_t low = read_byte(nes, ptr);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    nes->cpu.ea = low | high << 8;
}

static void idy_r(struct zc_nes *nes) {
    uint8_t ptr = eat_byte(nes);
    uint8_t low = read_byte(nes, ptr);
    bool overflow = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    if (overflow) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void idy_w(struct zc_nes *nes) {
    uint8_t ptr = eat_byte(nes);
    uint8_t low = read_byte(nes, ptr);
    bool overflow = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + overflow) << 8;
}

static void imm(struct zc_nes *nes) {
    nes->cpu.ea = nes->cpu.pc;
    ++nes->cpu.pc;
}

static void ind(struct zc_nes *nes) {
    uint8_t ptr_low = eat_byte(nes);
    uint8_t ptr_high = eat_byte(nes);
    uint8_t low = read_byte(nes, ptr_low | ptr_high << 8);
    uint8_t high = read_byte(nes, (uint8_t)(ptr_low + 1) | ptr_high << 8);
    nes->cpu.ea = low | high << 8;
}

static void zpg(struct zc_nes *nes) {
    nes->cpu.ea = eat_byte(nes);
}

static void zpx(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    read_byte(nes, low);
    nes->cpu.ea = (uint8_t)(low + nes->cpu.x);
}

static void zpy(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
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

static void sta(struct zc_nes *nes) {
    write_byte(nes, nes->cpu.ea, nes->cpu.a);
}

void cpu_step(struct zc_nes *nes) {
    uint8_t opc = eat_byte(nes);
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
    } else if (opc == 0xB1) {
        idy_r(nes);
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
    } else if (opc == 0x85) {
        zpg(nes);
        sta(nes);
    } else if (opc == 0x8D) {
        abu(nes);
        sta(nes);
    } else if (opc == 0x9D) {
        abx_w(nes);
        sta(nes);
    } else if (opc == 0x95) {
        zpx(nes);
        sta(nes);
    } else if (opc == 0x99) {
        aby_w(nes);
        sta(nes);
    } else if (opc == 0x81) {
        idx(nes);
        sta(nes);
    } else if (opc == 0x91) {
        idy_w(nes);
        sta(nes);
    }
}

void nes_step(struct zc_nes *nes) {
    cpu_step(nes);
}
