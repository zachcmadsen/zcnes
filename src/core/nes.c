#include <stdint.h>

#include "nes.h"

#define STACK_ADDR 0x0100
#define IRQ_VECTOR 0xFFFE

#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow) &&                                   \
    __has_builtin(__builtin_sub_overflow)
#define ZCNES_BUILTINS 1
#endif
#endif

static bool overflowing_add(uint8_t a, uint8_t b, uint8_t *res) {
#ifdef ZCNES_BUILTINS
    return __builtin_add_overflow(a, b, res);
#else
    *res = a + b;
    return *res < a;
#endif
}

static bool overflowing_sub(uint8_t a, uint8_t b, uint8_t *res) {
#ifdef ZCNES_BUILTINS
    return __builtin_sub_overflow(a, b, res);
#else
    *res = a - b;
    return *res > a;
#endif
}

static uint8_t read_byte(struct zc_nes *nes, uint16_t addr) {
#ifdef ZCNES_PROCESSOR_TESTS
    return nes->cpu.ram[addr];
#else
    (void)nes;
    (void)addr;
    return 0;
#endif
}

static uint8_t eat_byte(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.pc);
    ++nes->cpu.pc;
    return data;
}

static void write_byte(struct zc_nes *nes, uint16_t addr, uint8_t data) {
#ifdef ZCNES_PROCESSOR_TESTS
    nes->cpu.ram[addr] = data;
#else
    (void)nes;
    (void)addr;
    (void)data;
#endif
}

static void abu(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    uint8_t high = eat_byte(nes);
    nes->cpu.ea = low | high << 8;
}

static void abx_r(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.x, &low);
    uint8_t high = eat_byte(nes);
    if (nes->cpu.pg_cross) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
}

static void abx_w(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.x, &low);
    uint8_t high = eat_byte(nes);
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
}

static void aby_r(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = eat_byte(nes);
    if (nes->cpu.pg_cross) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
}

static void aby_w(struct zc_nes *nes) {
    uint8_t low = eat_byte(nes);
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = eat_byte(nes);
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
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
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    if (nes->cpu.pg_cross) {
        read_byte(nes, low | high << 8);
    }
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
}

static void idy_w(struct zc_nes *nes) {
    uint8_t ptr = eat_byte(nes);
    uint8_t low = read_byte(nes, ptr);
    nes->cpu.pg_cross = overflowing_add(low, nes->cpu.y, &low);
    uint8_t high = read_byte(nes, (uint8_t)(ptr + 1));
    read_byte(nes, low | high << 8);
    nes->cpu.ea = low | (uint8_t)(high + nes->cpu.pg_cross) << 8;
}

static void imm(struct zc_nes *nes) {
    nes->cpu.ea = nes->cpu.pc;
    ++nes->cpu.pc;
}

static void imp(struct zc_nes *nes) {
    (void)nes;
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

static void add(struct zc_nes *nes, uint8_t data) {
    uint8_t prev_a = nes->cpu.a;
    uint16_t sum = nes->cpu.a + data + nes->cpu.c;
    nes->cpu.a = (uint8_t)sum;
    nes->cpu.c = sum > 0xFF;
    nes->cpu.v = (prev_a ^ nes->cpu.a) & (data ^ nes->cpu.a) & 0x80;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void branch(struct zc_nes *nes, bool cond) {
    uint8_t offset = eat_byte(nes);
    if (cond) {
        read_byte(nes, nes->cpu.pc);
        uint16_t prev_pc = nes->cpu.pc;
        nes->cpu.pc += (int8_t)offset;
        if ((prev_pc & 0xFF00) != (nes->cpu.pc & 0xFF00)) {
            read_byte(nes, (uint8_t)(prev_pc + offset) | (prev_pc & 0xFF00));
        }
    }
}

static void compare(struct zc_nes *nes, uint8_t a, uint8_t b) {
    uint8_t res;
    bool overflow = overflowing_sub(a, b, &res);
    nes->cpu.c = !overflow;
    nes->cpu.z = res == 0;
    nes->cpu.n = res & 0x80;
}

static uint8_t peek(struct zc_nes *nes) {
    return read_byte(nes, STACK_ADDR + nes->cpu.s);
}

static uint8_t pop(struct zc_nes *nes) {
    ++nes->cpu.s;
    return read_byte(nes, STACK_ADDR + nes->cpu.s);
}

static void push(struct zc_nes *nes, uint8_t data) {
    // TODO: Stack updates could bypass the bus and write directly to RAM.
    // Would need to measure.
    write_byte(nes, STACK_ADDR + nes->cpu.s, data);
    --nes->cpu.s;
}

// https://github.com/TomHarte/ProcessorTests/issues/61
static void sh(struct zc_nes *nes, uint8_t data) {
    // TODO: Rewrite this as an if-else?
    uint8_t low = (uint8_t)nes->cpu.ea;
    uint8_t high = nes->cpu.ea >> 8;
    data = data & (high + !nes->cpu.pg_cross);
    high = nes->cpu.pg_cross ? data : high;
    write_byte(nes, low | high << 8, data);
}

static void adc(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    add(nes, data);
}

static void alr(struct zc_nes *nes) {
    nes->cpu.a &= read_byte(nes, nes->cpu.ea);
    nes->cpu.c = nes->cpu.a & 0x01;
    nes->cpu.a >>= 1;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void anc(struct zc_nes *nes) {
    nes->cpu.a &= read_byte(nes, nes->cpu.ea);
    nes->cpu.c = nes->cpu.a & 0x80;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

// clang-format off
static void and(struct zc_nes * nes) {
    nes->cpu.a &= read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}
// clang-format on

static void ane(struct zc_nes *nes) {
    (void)nes;
}

static void arr(struct zc_nes *nes) {
    nes->cpu.a &= read_byte(nes, nes->cpu.ea);
    nes->cpu.a = nes->cpu.a >> 1 | nes->cpu.c << 7;
    nes->cpu.c = nes->cpu.a & 0x40;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.v = nes->cpu.c ^ (nes->cpu.a & 0x20) >> 5;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void asl(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = data & 0x80;
    data <<= 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void asl_a(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.c = nes->cpu.a & 0x80;
    nes->cpu.a <<= 1;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void bcc(struct zc_nes *nes) {
    branch(nes, !nes->cpu.c);
}

static void bcs(struct zc_nes *nes) {
    branch(nes, nes->cpu.c);
}

static void beq(struct zc_nes *nes) {
    branch(nes, nes->cpu.z);
}

static void bit(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    nes->cpu.z = (nes->cpu.a & data) == 0;
    nes->cpu.v = data & 0x40;
    nes->cpu.n = data & 0x80;
}

static void bmi(struct zc_nes *nes) {
    branch(nes, nes->cpu.n);
}

static void bne(struct zc_nes *nes) {
    branch(nes, !nes->cpu.z);
}

static void bpl(struct zc_nes *nes) {
    branch(nes, !nes->cpu.n);
}

static void brk(struct zc_nes *nes) {
    eat_byte(nes);
    push(nes, nes->cpu.pc >> 8);
    push(nes, (uint8_t)nes->cpu.pc);
    // Set the B flag before pushing p onto the stack.
    uint8_t p = cpu_get_p(nes) | 1 << 4;
    push(nes, p);
    nes->cpu.i = true;
    uint8_t pcl = read_byte(nes, IRQ_VECTOR);
    uint8_t pch = read_byte(nes, IRQ_VECTOR + 1);
    nes->cpu.pc = pcl | pch << 8;
}

static void bvc(struct zc_nes *nes) {
    branch(nes, !nes->cpu.v);
}

static void bvs(struct zc_nes *nes) {
    branch(nes, nes->cpu.v);
}

static void clc(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.c = false;
}

static void cld(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.d = false;
}

static void cli(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.i = false;
}

static void clv(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.v = false;
}

static void cmp(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    compare(nes, nes->cpu.a, data);
}

static void cpx(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    compare(nes, nes->cpu.x, data);
}

static void cpy(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    compare(nes, nes->cpu.y, data);
}

static void dcp(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    --data;
    write_byte(nes, nes->cpu.ea, data);
    compare(nes, nes->cpu.a, data);
}

static void dec(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    --data;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void dex(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    --nes->cpu.x;
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void dey(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    --nes->cpu.y;
    nes->cpu.z = nes->cpu.y == 0;
    nes->cpu.n = nes->cpu.y & 0x80;
}

static void eor(struct zc_nes *nes) {
    nes->cpu.a ^= read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void inc(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    ++data;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void inx(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    ++nes->cpu.x;
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void iny(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    ++nes->cpu.y;
    nes->cpu.z = nes->cpu.y == 0;
    nes->cpu.n = nes->cpu.y & 0x80;
}

static void isc(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    ++data;
    write_byte(nes, nes->cpu.ea, data);
    add(nes, data ^ 0xFF);
}

static void jam(struct zc_nes *nes) {
    (void)nes;
}

static void jmp(struct zc_nes *nes) {
    nes->cpu.pc = nes->cpu.ea;
}

static void jsr(struct zc_nes *nes) {
    uint8_t pcl = eat_byte(nes);
    peek(nes);
    push(nes, nes->cpu.pc >> 8);
    push(nes, (uint8_t)nes->cpu.pc);
    uint8_t pch = eat_byte(nes);
    nes->cpu.pc = pcl | pch << 8;
}

static void las(struct zc_nes *nes) {
    nes->cpu.a = read_byte(nes, nes->cpu.ea) & nes->cpu.s;
    nes->cpu.x = nes->cpu.a;
    nes->cpu.s = nes->cpu.a;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void lax(struct zc_nes *nes) {
    nes->cpu.a = read_byte(nes, nes->cpu.ea);
    nes->cpu.x = nes->cpu.a;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
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

static void ldy(struct zc_nes *nes) {
    nes->cpu.y = read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.y == 0;
    nes->cpu.n = nes->cpu.y & 0x80;
}

static void lsr(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = data & 0x01;
    data >>= 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void lsr_a(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.c = nes->cpu.a & 0x01;
    nes->cpu.a >>= 1;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void lxa(struct zc_nes *nes) {
    (void)nes;
}

static void nop(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.ea);
}

static void ora(struct zc_nes *nes) {
    nes->cpu.a |= read_byte(nes, nes->cpu.ea);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void pha(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    push(nes, nes->cpu.a);
}

static void php(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    uint8_t p = cpu_get_p(nes) | 1 << 4;
    push(nes, p);
}

static void pla(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    peek(nes);
    nes->cpu.a = pop(nes);
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void plp(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    peek(nes);
    bool prev_b = nes->cpu.b;
    cpu_set_p(nes, pop(nes));
    nes->cpu.b = prev_b;
}

static void rla(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    bool c = data & 0x80;
    data = nes->cpu.c | data << 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.a &= data;
    nes->cpu.c = c;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void rol(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    bool c = data & 0x80;
    data = nes->cpu.c | data << 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = c;
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void rol_a(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    bool c = nes->cpu.a & 0x80;
    nes->cpu.a = nes->cpu.c | nes->cpu.a << 1;
    nes->cpu.c = c;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void ror(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    bool c = data & 0x01;
    data = data >> 1 | nes->cpu.c << 7;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = c;
    nes->cpu.z = data == 0;
    nes->cpu.n = data & 0x80;
}

static void ror_a(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    bool c = nes->cpu.a & 0x01;
    nes->cpu.a = nes->cpu.a >> 1 | nes->cpu.c << 7;
    nes->cpu.c = c;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void rra(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    bool c = data & 0x01;
    data = data >> 1 | nes->cpu.c << 7;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = c;
    add(nes, data);
}

static void rti(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    peek(nes);
    bool prev_b = nes->cpu.b;
    cpu_set_p(nes, pop(nes));
    nes->cpu.b = prev_b;
    uint8_t pcl = pop(nes);
    uint8_t pch = pop(nes);
    nes->cpu.pc = pcl | pch << 8;
}

static void rts(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    peek(nes);
    uint8_t pcl = pop(nes);
    uint8_t pch = pop(nes);
    nes->cpu.pc = pcl | pch << 8;
    eat_byte(nes);
}

static void sax(struct zc_nes *nes) {
    write_byte(nes, nes->cpu.ea, nes->cpu.a & nes->cpu.x);
}

static void sbc(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    add(nes, data ^ 0xFF);
}

static void sbx(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    bool overflow = overflowing_sub(nes->cpu.a & nes->cpu.x, data, &nes->cpu.x);
    nes->cpu.c = !overflow;
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void sec(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.c = true;
}

static void sed(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.d = true;
}

static void sei(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.i = true;
}

static void sha(struct zc_nes *nes) {
    sh(nes, nes->cpu.a & nes->cpu.x);
}

static void shx(struct zc_nes *nes) {
    sh(nes, nes->cpu.x);
}

static void shy(struct zc_nes *nes) {
    sh(nes, nes->cpu.y);
}

static void slo(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = data & 0x80;
    data <<= 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.a |= data;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void sre(struct zc_nes *nes) {
    uint8_t data = read_byte(nes, nes->cpu.ea);
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.c = data & 0x01;
    data >>= 1;
    write_byte(nes, nes->cpu.ea, data);
    nes->cpu.a ^= data;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void sta(struct zc_nes *nes) {
    write_byte(nes, nes->cpu.ea, nes->cpu.a);
}

static void stx(struct zc_nes *nes) {
    write_byte(nes, nes->cpu.ea, nes->cpu.x);
}

static void sty(struct zc_nes *nes) {
    write_byte(nes, nes->cpu.ea, nes->cpu.y);
}

static void tas(struct zc_nes *nes) {
    nes->cpu.s = nes->cpu.a & nes->cpu.x;
    sh(nes, nes->cpu.s);
}

static void tax(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.x = nes->cpu.a;
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void tay(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.y = nes->cpu.a;
    nes->cpu.z = nes->cpu.y == 0;
    nes->cpu.n = nes->cpu.y & 0x80;
}

static void tsx(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.x = nes->cpu.s;
    nes->cpu.z = nes->cpu.x == 0;
    nes->cpu.n = nes->cpu.x & 0x80;
}

static void txa(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.a = nes->cpu.x;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

static void txs(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.s = nes->cpu.x;
}

static void tya(struct zc_nes *nes) {
    read_byte(nes, nes->cpu.pc);
    nes->cpu.a = nes->cpu.y;
    nes->cpu.z = nes->cpu.a == 0;
    nes->cpu.n = nes->cpu.a & 0x80;
}

uint8_t cpu_get_p(struct zc_nes *nes) {
    // Bit 5 is always 1.
    return nes->cpu.c | (nes->cpu.z << 1) | (nes->cpu.i << 2) |
           (nes->cpu.d << 3) | (nes->cpu.b << 4) | (1 << 5) |
           (nes->cpu.v << 6) | (nes->cpu.n << 7);
}

void cpu_set_p(struct zc_nes *nes, uint8_t p) {
    nes->cpu.c = p & 0x01;
    nes->cpu.z = p & 0x02;
    nes->cpu.i = p & 0x04;
    nes->cpu.d = p & 0x08;
    nes->cpu.b = p & 0x10;
    nes->cpu.v = p & 0x40;
    nes->cpu.n = p & 0x80;
}

struct opc_info {
    void (*mode)(struct zc_nes *);
    void (*instr)(struct zc_nes *);
};

struct opc_info opc_lut[256] = {
    {imp, brk},   {idx, ora},   {imp, jam},   {idx, slo},   {zpg, nop},
    {zpg, ora},   {zpg, asl},   {zpg, slo},   {imp, php},   {imm, ora},
    {imp, asl_a}, {imm, anc},   {abu, nop},   {abu, ora},   {abu, asl},
    {abu, slo},   {imp, bpl},   {idy_r, ora}, {imp, jam},   {idy_w, slo},
    {zpx, nop},   {zpx, ora},   {zpx, asl},   {zpx, slo},   {imp, clc},
    {aby_r, ora}, {imp, nop},   {aby_w, slo}, {abx_r, nop}, {abx_r, ora},
    {abx_w, asl}, {abx_w, slo}, {imp, jsr},   {idx, and},   {imp, jam},
    {idx, rla},   {zpg, bit},   {zpg, and},   {zpg, rol},   {zpg, rla},
    {imp, plp},   {imm, and},   {imp, rol_a}, {imm, anc},   {abu, bit},
    {abu, and},   {abu, rol},   {abu, rla},   {imp, bmi},   {idy_r, and},
    {imp, jam},   {idy_w, rla}, {zpx, nop},   {zpx, and},   {zpx, rol},
    {zpx, rla},   {imp, sec},   {aby_r, and}, {imp, nop},   {aby_w, rla},
    {abx_r, nop}, {abx_r, and}, {abx_w, rol}, {abx_w, rla}, {imp, rti},
    {idx, eor},   {imp, jam},   {idx, sre},   {zpg, nop},   {zpg, eor},
    {zpg, lsr},   {zpg, sre},   {imp, pha},   {imm, eor},   {imp, lsr_a},
    {imm, alr},   {abu, jmp},   {abu, eor},   {abu, lsr},   {abu, sre},
    {imp, bvc},   {idy_r, eor}, {imp, jam},   {idy_w, sre}, {zpx, nop},
    {zpx, eor},   {zpx, lsr},   {zpx, sre},   {imp, cli},   {aby_r, eor},
    {imp, nop},   {aby_w, sre}, {abx_r, nop}, {abx_r, eor}, {abx_w, lsr},
    {abx_w, sre}, {imp, rts},   {idx, adc},   {imp, jam},   {idx, rra},
    {zpg, nop},   {zpg, adc},   {zpg, ror},   {zpg, rra},   {imp, pla},
    {imm, adc},   {imp, ror_a}, {imm, arr},   {ind, jmp},   {abu, adc},
    {abu, ror},   {abu, rra},   {imp, bvs},   {idy_r, adc}, {imp, jam},
    {idy_w, rra}, {zpx, nop},   {zpx, adc},   {zpx, ror},   {zpx, rra},
    {imp, sei},   {aby_r, adc}, {imp, nop},   {aby_w, rra}, {abx_r, nop},
    {abx_r, adc}, {abx_w, ror}, {abx_w, rra}, {imm, nop},   {idx, sta},
    {imm, nop},   {idx, sax},   {zpg, sty},   {zpg, sta},   {zpg, stx},
    {zpg, sax},   {imp, dey},   {imm, nop},   {imp, txa},   {imp, ane},
    {abu, sty},   {abu, sta},   {abu, stx},   {abu, sax},   {imp, bcc},
    {idy_w, sta}, {imp, jam},   {idy_w, sha}, {zpx, sty},   {zpx, sta},
    {zpy, stx},   {zpy, sax},   {imp, tya},   {aby_w, sta}, {imp, txs},
    {aby_w, tas}, {abx_w, shy}, {abx_w, sta}, {aby_w, shx}, {aby_w, sha},
    {imm, ldy},   {idx, lda},   {imm, ldx},   {idx, lax},   {zpg, ldy},
    {zpg, lda},   {zpg, ldx},   {zpg, lax},   {imp, tay},   {imm, lda},
    {imp, tax},   {imm, lxa},   {abu, ldy},   {abu, lda},   {abu, ldx},
    {abu, lax},   {imp, bcs},   {idy_r, lda}, {imp, jam},   {idy_r, lax},
    {zpx, ldy},   {zpx, lda},   {zpy, ldx},   {zpy, lax},   {imp, clv},
    {aby_r, lda}, {imp, tsx},   {aby_r, las}, {abx_r, ldy}, {abx_r, lda},
    {aby_r, ldx}, {aby_r, lax}, {imm, cpy},   {idx, cmp},   {imm, nop},
    {idx, dcp},   {zpg, cpy},   {zpg, cmp},   {zpg, dec},   {zpg, dcp},
    {imp, iny},   {imm, cmp},   {imp, dex},   {imm, sbx},   {abu, cpy},
    {abu, cmp},   {abu, dec},   {abu, dcp},   {imp, bne},   {idy_r, cmp},
    {imp, jam},   {idy_w, dcp}, {zpx, nop},   {zpx, cmp},   {zpx, dec},
    {zpx, dcp},   {imp, cld},   {aby_r, cmp}, {imp, nop},   {aby_w, dcp},
    {abx_r, nop}, {abx_r, cmp}, {abx_w, dec}, {abx_w, dcp}, {imm, cpx},
    {idx, sbc},   {imm, nop},   {idx, isc},   {zpg, cpx},   {zpg, sbc},
    {zpg, inc},   {zpg, isc},   {imp, inx},   {imm, sbc},   {imp, nop},
    {imm, sbc},   {abu, cpx},   {abu, sbc},   {abu, inc},   {abu, isc},
    {imp, beq},   {idy_r, sbc}, {imp, jam},   {idy_w, isc}, {zpx, nop},
    {zpx, sbc},   {zpx, inc},   {zpx, isc},   {imp, sed},   {aby_r, sbc},
    {imp, nop},   {aby_w, isc}, {abx_r, nop}, {abx_r, sbc}, {abx_w, inc},
    {abx_w, isc},
};

void cpu_step(struct zc_nes *nes) {
    uint8_t opc = eat_byte(nes);
    opc_lut[opc].mode(nes);
    opc_lut[opc].instr(nes);
}

void nes_step(struct zc_nes *nes) {
    cpu_step(nes);
}
