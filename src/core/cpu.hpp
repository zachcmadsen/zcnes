#pragma once

#include <bit>
#include <cstdint>

#include "bit.hpp"
#include "concepts.hpp"
#include "num.hpp"

namespace zcnes
{

// This is incomplete because, for now, it's only used for single steps tests.
struct CpuState
{
    std::uint16_t pc{};
    std::uint8_t a{};
    std::uint8_t x{};
    std::uint8_t y{};
    std::uint8_t s{};
    std::uint8_t p{};
};

/// A Ricoh 6502 emulator.
template <Addressable T> class Cpu
{
  public:
    explicit Cpu(T *bus) : bus{bus} {};

    void reset()
    {
        read_byte(pc);
        peek();
        s -= 1;
        peek();
        s -= 1;
        peek();
        s -= 1;
        p.i = true;
        const auto pc_low = read_byte(reset_vector);
        const auto pc_high = read_byte(reset_vector + 1);
        pc = bit::combine(pc_high, pc_low);
    }

    void step()
    {
        constexpr auto read = false;
        constexpr auto write = true;

        const auto opcode = next_byte();
        // clang-format off
        switch (opcode)
        {
        case 0x00:               brk();   break;
        case 0x01: idx();        ora();   break;
        case 0x03: idx();        slo();   break;
        case 0x04: zpg();        nop();   break;
        case 0x05: zpg();        ora();   break;
        case 0x06: zpg();        asl();   break;
        case 0x07: zpg();        slo();   break;
        case 0x08:               php();   break;
        case 0x09: imm();        ora();   break;
        case 0x0A:               asl_a(); break;
        case 0x0B: imm();        anc();   break;
        case 0x0C: abs();        nop();   break;
        case 0x0D: abs();        ora();   break;
        case 0x0E: abs();        asl();   break;
        case 0x0F: abs();        slo();   break;
        case 0x10:               bpl();   break;
        case 0x11: idy<read>();  ora();   break;
        case 0x13: idy<write>(); slo();   break;
        case 0x14: zpx();        nop();   break;
        case 0x15: zpx();        ora();   break;
        case 0x16: zpx();        asl();   break;
        case 0x17: zpx();        slo();   break;
        case 0x18:               clc();   break;
        case 0x19: aby<read>();  ora();   break;
        case 0x1A: imp();        nop();   break;
        case 0x1B: aby<write>(); slo();   break;
        case 0x1C: abx<read>();  nop();   break;
        case 0x1D: abx<read>();  ora();   break;
        case 0x1E: abx<write>(); asl();   break;
        case 0x1F: abx<write>(); slo();   break;
        case 0x20:               jsr();   break;
        case 0x21: idx();        and_();  break;
        case 0x23: idx();        rla();   break;
        case 0x24: zpg();        bit();   break;
        case 0x25: zpg();        and_();  break;
        case 0x26: zpg();        rol();   break;
        case 0x27: zpg();        rla();   break;
        case 0x28:               plp();   break;
        case 0x29: imm();        and_();  break;
        case 0x2A:               rol_a(); break;
        case 0x2B: imm();        anc();   break;
        case 0x2C: abs();        bit();   break;
        case 0x2D: abs();        and_();  break;
        case 0x2E: abs();        rol();   break;
        case 0x2F: abs();        rla();   break;
        case 0x30:               bmi();   break;
        case 0x31: idy<read>();  and_();  break;
        case 0x33: idy<write>(); rla();   break;
        case 0x34: zpx();        nop();   break;
        case 0x35: zpx();        and_();  break;
        case 0x36: zpx();        rol();   break;
        case 0x37: zpx();        rla();   break;
        case 0x38:               sec();   break;
        case 0x39: aby<read>();  and_();  break;
        case 0x3A: imp();        nop();   break;
        case 0x3B: aby<write>(); rla();   break;
        case 0x3C: abx<read>();  nop();   break;
        case 0x3D: abx<read>();  and_();  break;
        case 0x3E: abx<write>(); rol();   break;
        case 0x3F: abx<write>(); rla();   break;
        case 0x40:               rti();   break;
        case 0x41: idx();        eor();   break;
        case 0x43: idx();        sre();   break;
        case 0x44: zpg();        nop();   break;
        case 0x45: zpg();        eor();   break;
        case 0x46: zpg();        lsr();   break;
        case 0x47: zpg();        sre();   break;
        case 0x48:               pha();   break;
        case 0x49: imm();        eor();   break;
        case 0x4A:               lsr_a(); break;
        case 0x4B: imm();        alr();   break;
        case 0x4C: abs();        jmp();   break;
        case 0x4D: abs();        eor();   break;
        case 0x4E: abs();        lsr();   break;
        case 0x4F: abs();        sre();   break;
        case 0x50:               bvc();   break;
        case 0x51: idy<read>();  eor();   break;
        case 0x53: idy<write>(); sre();   break;
        case 0x54: zpx();        nop();   break;
        case 0x55: zpx();        eor();   break;
        case 0x56: zpx();        lsr();   break;
        case 0x57: zpx();        sre();   break;
        case 0x58:               cli();   break;
        case 0x59: aby<read>();  eor();   break;
        case 0x5A: imp();        nop();   break;
        case 0x5B: aby<write>(); sre();   break;
        case 0x5C: abx<read>();  nop();   break;
        case 0x5D: abx<read>();  eor();   break;
        case 0x5E: abx<write>(); lsr();   break;
        case 0x5F: abx<write>(); sre();   break;
        case 0x60:               rts();   break;
        case 0x61: idx();        adc();   break;
        case 0x63: idx();        rra();   break;
        case 0x64: zpg();        nop();   break;
        case 0x65: zpg();        adc();   break;
        case 0x66: zpg();        ror();   break;
        case 0x67: zpg();        rra();   break;
        case 0x68:               pla();   break;
        case 0x69: imm();        adc();   break;
        case 0x6A:               ror_a(); break;
        case 0x6B: imm();        arr();   break;
        case 0x6C: ind();        jmp();   break;
        case 0x6D: abs();        adc();   break;
        case 0x6E: abs();        ror();   break;
        case 0x6F: abs();        rra();   break;
        case 0x70:               bvs();   break;
        case 0x71: idy<read>();  adc();   break;
        case 0x73: idy<write>(); rra();   break;
        case 0x74: zpx();        nop();   break;
        case 0x75: zpx();        adc();   break;
        case 0x76: zpx();        ror();   break;
        case 0x77: zpx();        rra();   break;
        case 0x78:               sei();   break;
        case 0x79: aby<read>();  adc();   break;
        case 0x7A: imp();        nop();   break;
        case 0x7B: aby<write>(); rra();   break;
        case 0x7C: abx<read>();  nop();   break;
        case 0x7D: abx<read>();  adc();   break;
        case 0x7E: abx<write>(); ror();   break;
        case 0x7F: abx<write>(); rra();   break;
        case 0x80: imm();        nop();   break;
        case 0x81: idx();        sta();   break;
        case 0x82: imm();        nop();   break;
        case 0x83: idx();        sax();   break;
        case 0x84: zpg();        sty();   break;
        case 0x85: zpg();        sta();   break;
        case 0x86: zpg();        stx();   break;
        case 0x87: zpg();        sax();   break;
        case 0x88:               dey();   break;
        case 0x89: imm();        nop();   break;
        case 0x8A:               txa();   break;
        case 0x8B:                        break;
        case 0x8C: abs();        sty();   break;
        case 0x8D: abs();        sta();   break;
        case 0x8E: abs();        stx();   break;
        case 0x8F: abs();        sax();   break;
        case 0x90:               bcc();   break;
        case 0x91: idy<write>(); sta();   break;
        case 0x93: idy<write>(); sha();   break;
        case 0x94: zpx();        sty();   break;
        case 0x95: zpx();        sta();   break;
        case 0x96: zpy();        stx();   break;
        case 0x97: zpy();        sax();   break;
        case 0x98:               tya();   break;
        case 0x99: aby<write>(); sta();   break;
        case 0x9A:               txs();   break;
        case 0x9B: aby<write>(); shs();   break;
        case 0x9C: abx<write>(); shy();   break;
        case 0x9D: abx<write>(); sta();   break;
        case 0x9E: aby<write>(); shx();   break;
        case 0x9F: aby<write>(); sha();   break;
        case 0xA0: imm();        ldy();   break;
        case 0xA1: idx();        lda();   break;
        case 0xA2: imm();        ldx();   break;
        case 0xA3: idx();        lax();   break;
        case 0xA4: zpg();        ldy();   break;
        case 0xA5: zpg();        lda();   break;
        case 0xA6: zpg();        ldx();   break;
        case 0xA7: zpg();        lax();   break;
        case 0xA8:               tay();   break;
        case 0xA9: imm();        lda();   break;
        case 0xAA:               tax();   break;
        case 0xAB: imm();        lxa();   break;
        case 0xAC: abs();        ldy();   break;
        case 0xAD: abs();        lda();   break;
        case 0xAE: abs();        ldx();   break;
        case 0xAF: abs();        lax();   break;
        case 0xB0:               bcs();   break;
        case 0xB1: idy<read>();  lda();   break;
        case 0xB3: idy<read>();  lax();   break;
        case 0xB4: zpx();        ldy();   break;
        case 0xB5: zpx();        lda();   break;
        case 0xB6: zpy();        ldx();   break;
        case 0xB7: zpy();        lax();   break;
        case 0xB8:               clv();   break;
        case 0xB9: aby<read>();  lda();   break;
        case 0xBA:               tsx();   break;
        case 0xBB: aby<read>();  las();   break;
        case 0xBC: abx<read>();  ldy();   break;
        case 0xBD: abx<read>();  lda();   break;
        case 0xBE: aby<read>();  ldx();   break;
        case 0xBF: aby<read>();  lax();   break;
        case 0xC0: imm();        cpy();   break;
        case 0xC1: idx();        cmp();   break;
        case 0xC2: imm();        nop();   break;
        case 0xC3: idx();        dcp();   break;
        case 0xC4: zpg();        cpy();   break;
        case 0xC5: zpg();        cmp();   break;
        case 0xC6: zpg();        dec();   break;
        case 0xC7: zpg();        dcp();   break;
        case 0xC8:               iny();   break;
        case 0xC9: imm();        cmp();   break;
        case 0xCA:               dex();   break;
        case 0xCB: imm();        sbx();   break;
        case 0xCC: abs();        cpy();   break;
        case 0xCD: abs();        cmp();   break;
        case 0xCE: abs();        dec();   break;
        case 0xCF: abs();        dcp();   break;
        case 0xD0:               bne();   break;
        case 0xD1: idy<read>();  cmp();   break;
        case 0xD3: idy<write>(); dcp();   break;
        case 0xD4: zpx();        nop();   break;
        case 0xD5: zpx();        cmp();   break;
        case 0xD6: zpx();        dec();   break;
        case 0xD7: zpx();        dcp();   break;
        case 0xD8:               cld();   break;
        case 0xD9: aby<read>();  cmp();   break;
        case 0xDA: imp();        nop();   break;
        case 0xDB: aby<write>(); dcp();   break;
        case 0xDC: abx<read>();  nop();   break;
        case 0xDD: abx<read>();  cmp();   break;
        case 0xDE: abx<write>(); dec();   break;
        case 0xDF: abx<write>(); dcp();   break;
        case 0xE0: imm();        cpx();   break;
        case 0xE1: idx();        sbc();   break;
        case 0xE2: imm();        nop();   break;
        case 0xE3: idx();        isc();   break;
        case 0xE4: zpg();        cpx();   break;
        case 0xE5: zpg();        sbc();   break;
        case 0xE6: zpg();        inc();   break;
        case 0xE7: zpg();        isc();   break;
        case 0xE8:               inx();   break;
        case 0xE9: imm();        sbc();   break;
        case 0xEA: imp();        nop();   break;
        case 0xEB: imm();        sbc();   break;
        case 0xEC: abs();        cpx();   break;
        case 0xED: abs();        sbc();   break;
        case 0xEE: abs();        inc();   break;
        case 0xEF: abs();        isc();   break;
        case 0xF0:               beq();   break;
        case 0xF1: idy<read>();  sbc();   break;
        case 0xF3: idy<write>(); isc();   break;
        case 0xF4: zpx();        nop();   break;
        case 0xF5: zpx();        sbc();   break;
        case 0xF6: zpx();        inc();   break;
        case 0xF7: zpx();        isc();   break;
        case 0xF8:               sed();   break;
        case 0xF9: aby<read>();  sbc();   break;
        case 0xFA: imp();        nop();   break;
        case 0xFB: aby<write>(); isc();   break;
        case 0xFC: abx<read>();  nop();   break;
        case 0xFD: abx<read>();  sbc();   break;
        case 0xFE: abx<write>(); inc();   break;
        case 0xFF: abx<write>(); isc();   break;
        }
        // clang-format on

        if (prev_nmi_pending)
        {
            nmi_pending = false;
            nmi();
        }
    }

    void set_nmi_pin(bool status)
    {
        nmi_pin = status;
    }

    void load_state(const CpuState &state)
    {
        pc = state.pc;
        a = state.a;
        x = state.x;
        y = state.y;
        s = state.s;
        p = std::bit_cast<Status>(state.p);
    }

    void save_state(CpuState &state) const
    {
        state.pc = pc;
        state.a = a;
        state.x = x;
        state.y = y;
        state.s = s;
        state.p = std::bit_cast<std::uint8_t>(p);
    }

  private:
    static constexpr std::uint16_t stack_addr = 0x0100;

    static constexpr std::uint16_t nmi_vector = 0xFFFA;
    static constexpr std::uint16_t reset_vector = 0xFFFC;
    static constexpr std::uint16_t irq_vector = 0xFFFE;

    struct Status
    {
        bool c : 1;
        bool z : 1;
        bool i : 1;
        bool d : 1;
        bool b : 1;
        bool u : 1;
        bool v : 1;
        bool n : 1;
    };
    static_assert(sizeof(Status) == 1);

    std::uint16_t pc{0};
    std::uint8_t a{0};
    std::uint8_t x{0};
    std::uint8_t y{0};
    std::uint8_t s{0xFD};
    Status p{false, false, true, false, false, true, false, false};

    std::uint16_t addr{0};
    // Track page crossings for SHA, SHX, SHY, and SHS instructions.
    bool had_page_cross{false};

    bool nmi_pin{false};
    bool prev_nmi_pin{false};
    bool nmi_pending{false};
    bool prev_nmi_pending{false};

    T *bus;

    void poll_nmi()
    {
        prev_nmi_pending = nmi_pending;
        if (!prev_nmi_pin && nmi_pin)
        {
            nmi_pending = true;
        }
        prev_nmi_pin = nmi_pin;
    }

    void nmi()
    {
        read_byte(pc);
        push(pc >> 8);
        push(static_cast<std::uint8_t>(pc));
        push(std::bit_cast<std::uint8_t>(p));
        p.i = true;
        const auto pc_low = read_byte(nmi_vector);
        const auto pc_high = read_byte(nmi_vector + 1);
        pc = bit::combine(pc_high, pc_low);
    }

    std::uint8_t read_byte(std::uint16_t addr)
    {
        const auto data = bus->read_byte(addr);
        poll_nmi();
        return data;
    }

    void write_byte(std::uint16_t addr, std::uint8_t data)
    {
        bus->write_byte(addr, data);
        poll_nmi();
    }

    std::uint8_t next_byte()
    {
        const auto data = read_byte(pc);
        pc += 1;
        return data;
    }

    void set_z_and_n(std::uint8_t data)
    {
        p.z = data == 0;
        p.n = bit::msb(data);
    }

    void push(std::uint8_t data)
    {
        write_byte(stack_addr + s, data);
        s -= 1;
    }

    std::uint8_t pop()
    {
        s += 1;
        return read_byte(stack_addr + s);
    }

    std::uint8_t peek()
    {
        return read_byte(stack_addr + s);
    }

    void add(std::uint8_t data)
    {
        const auto prev_a = a;
        std::uint8_t carry = 0;
        a = num::carrying_add(a, data, p.c, &carry);
        p.c = carry;
        p.v = bit::msb((prev_a ^ a) & (data ^ a));
        set_z_and_n(a);
    }

    void branch(bool cond)
    {
        const auto offset = next_byte();
        if (cond)
        {
            read_byte(pc);
            const auto prev_pc = pc;
            pc += static_cast<std::int8_t>(offset);
            const auto did_page_cross = (prev_pc & 0xFF00) != (pc & 0xFF00);
            if (did_page_cross)
            {
                const auto low = static_cast<std::uint8_t>(prev_pc + offset);
                const auto high = static_cast<std::uint8_t>(prev_pc >> 8);
                read_byte(bit::combine(high, low));
            }
        }
    }

    void compare(std::uint8_t lhs, std::uint8_t rhs)
    {
        std::uint8_t res = 0;
        const auto overflow = num::overflowing_sub(lhs, rhs, &res);
        p.c = !overflow;
        set_z_and_n(res);
    }

    /// A helper function for SHA, SHX, SHY, AND SHS.
    ///
    /// See https://github.com/SingleStepTests/ProcessorTests/issues/61 for
    /// how the instructions work.
    void sh(std::uint8_t data)
    {
        // TODO: Write this better?
        const auto low = static_cast<std::uint8_t>(addr);
        std::uint8_t high = addr >> 8;
        data &= high + static_cast<std::uint8_t>(!had_page_cross);
        high = had_page_cross ? data : high;
        write_byte(bit::combine(high, low), data);
    }

    void abs()
    {
        const auto low = next_byte();
        const auto high = next_byte();
        addr = bit::combine(high, low);
    }

    template <bool write> void abx()
    {
        auto low = next_byte();
        const auto overflow = num::overflowing_add(low, x, &low);
        const auto high = next_byte();
        if (write || overflow)
        {
            read_byte(bit::combine(high, low));
        }
        addr = bit::combine(high + overflow, low);
        had_page_cross = overflow;
    }

    template <bool write> void aby()
    {
        auto low = next_byte();
        const auto overflow = num::overflowing_add(low, y, &low);
        const auto high = next_byte();
        if (write || overflow)
        {
            read_byte(bit::combine(high, low));
        }
        addr = bit::combine(high + overflow, low);
        had_page_cross = overflow;
    }

    void idx()
    {
        auto ptr = next_byte();
        read_byte(ptr);
        ptr += x;
        const auto low = read_byte(ptr);
        ptr += 1;
        const auto high = read_byte(ptr);
        addr = bit::combine(high, low);
    }

    template <bool write> void idy()
    {
        auto ptr = next_byte();
        auto low = read_byte(ptr);
        ptr += 1;
        const auto high = read_byte(ptr);
        const auto overflow = num::overflowing_add(low, y, &low);
        if (write || overflow)
        {
            read_byte(bit::combine(high, low));
        }
        addr = bit::combine(high + overflow, low);
        had_page_cross = overflow;
    }

    void imm()
    {
        addr = pc;
        pc += 1;
    }

    // This is only meant to be used with `nop`.
    void imp()
    {
        addr = pc;
    }

    void ind()
    {
        const auto ptr_low = next_byte();
        const auto ptr_high = next_byte();
        const auto low = read_byte(bit::combine(ptr_high, ptr_low));
        const auto high = read_byte(bit::combine(ptr_high, ptr_low + 1));
        addr = bit::combine(high, low);
    }

    void zpg()
    {
        addr = next_byte();
    }

    void zpx()
    {
        auto addr = next_byte();
        read_byte(addr);
        addr += x;
        this->addr = addr;
    }

    void zpy()
    {
        auto addr = next_byte();
        read_byte(addr);
        addr += y;
        this->addr = addr;
    }

    void adc()
    {
        const auto data = read_byte(addr);
        add(data);
    }

    void alr()
    {
        a &= read_byte(addr);
        const auto carry = bit::lsb(a);
        a >>= 1;
        p.c = carry;
        set_z_and_n(a);
    }

    void anc()
    {
        a &= read_byte(addr);
        p.c = bit::msb(a);
        set_z_and_n(a);
    }

    void and_()
    {
        a &= read_byte(addr);
        set_z_and_n(a);
    }

    void ane()
    {
    }

    void arr()
    {
        a &= read_byte(addr);
        a >>= 1;
        a |= p.c << 7;
        p.c = bit::bit<6>(a);
        p.v = p.c ^ bit::bit<5>(a);
        set_z_and_n(a);
    }

    void asl()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::msb(data);
        data <<= 1;
        write_byte(addr, data);
        p.c = carry;
        set_z_and_n(data);
    }

    void asl_a()
    {
        read_byte(pc);
        const auto carry = bit::msb(a);
        a <<= 1;
        p.c = carry;
        set_z_and_n(a);
    }

    void bcc()
    {
        branch(!p.c);
    }

    void bcs()
    {
        branch(p.c);
    }

    void beq()
    {
        branch(p.z);
    }

    void bit()
    {
        const auto data = read_byte(addr);
        p.z = !(data & a);
        p.v = bit::bit<6>(data);
        p.n = bit::msb(data);
    }

    void bmi()
    {
        branch(p.n);
    }

    void bne()
    {
        branch(!p.z);
    }

    void bpl()
    {
        branch(!p.n);
    }

    void brk()
    {
        next_byte();
        push(pc >> 8);
        push(static_cast<std::uint8_t>(pc));
        // Push P with the B flag set.
        push(std::bit_cast<std::uint8_t>(p) | 1 << 4);
        p.i = true;
        const auto pc_low = read_byte(irq_vector);
        const auto pc_high = read_byte(irq_vector + 1);
        pc = bit::combine(pc_high, pc_low);
    }

    void bvc()
    {
        branch(!p.v);
    }

    void bvs()
    {
        branch(p.v);
    }

    void clc()
    {
        read_byte(pc);
        p.c = false;
    }

    void cld()
    {
        read_byte(pc);
        p.d = false;
    }

    void cli()
    {
        read_byte(pc);
        p.i = false;
    }

    void clv()
    {
        read_byte(pc);
        p.v = false;
    }

    void cmp()
    {
        const auto data = read_byte(addr);
        compare(a, data);
    }

    void cpx()
    {
        const auto data = read_byte(addr);
        compare(x, data);
    }

    void cpy()
    {
        const auto data = read_byte(addr);
        compare(y, data);
    }

    void dcp()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        data -= 1;
        write_byte(addr, data);
        compare(a, data);
    }

    void dec()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        data -= 1;
        write_byte(addr, data);
        set_z_and_n(data);
    }

    void dex()
    {
        read_byte(pc);
        x -= 1;
        set_z_and_n(x);
    }

    void dey()
    {
        read_byte(pc);
        y -= 1;
        set_z_and_n(y);
    }

    void eor()
    {
        a ^= read_byte(addr);
        set_z_and_n(a);
    }

    void inc()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        data += 1;
        write_byte(addr, data);
        set_z_and_n(data);
    }

    void inx()
    {
        read_byte(pc);
        x += 1;
        set_z_and_n(x);
    }

    void iny()
    {
        read_byte(pc);
        y += 1;
        set_z_and_n(y);
    }

    void isc()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        data += 1;
        write_byte(addr, data);
        add(data ^ 0xFF);
    }

    void jam()
    {
    }

    void jmp()
    {
        pc = addr;
    }

    void jsr()
    {
        const auto pc_low = next_byte();
        peek();
        push(pc >> 8);
        push(static_cast<std::uint8_t>(pc));
        const auto pc_high = next_byte();
        pc = bit::combine(pc_high, pc_low);
    }

    void las()
    {
        a = read_byte(addr) & s;
        x = a;
        s = a;
        set_z_and_n(a);
    }

    void lax()
    {
        a = read_byte(addr);
        x = a;
        set_z_and_n(a);
    }

    void lda()
    {
        a = read_byte(addr);
        set_z_and_n(a);
    }

    void ldx()
    {
        x = read_byte(addr);
        set_z_and_n(x);
    }

    void ldy()
    {
        y = read_byte(addr);
        set_z_and_n(y);
    }

    void lsr()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::lsb(data);
        data >>= 1;
        write_byte(addr, data);
        p.c = carry;
        set_z_and_n(data);
    }

    void lsr_a()
    {
        read_byte(pc);
        const auto carry = bit::lsb(a);
        a >>= 1;
        p.c = carry;
        set_z_and_n(a);
    }

    void lxa()
    {
        // Store 0xFF & operand in A and X.
        a = read_byte(addr);
        x = a;
        set_z_and_n(a);
    }

    void nop()
    {
        read_byte(addr);
    }

    void ora()
    {
        a |= read_byte(addr);
        set_z_and_n(a);
    }

    void pha()
    {
        read_byte(pc);
        push(a);
    }

    void php()
    {
        read_byte(pc);
        // Push P with the B and U flags set.
        push(std::bit_cast<std::uint8_t>(p) | 1 << 5 | 1 << 4);
    }

    void pla()
    {
        read_byte(pc);
        peek();
        a = pop();
        set_z_and_n(a);
    }

    void plp()
    {
        read_byte(pc);
        peek();
        auto new_p = std::bit_cast<Status>(pop());
        new_p.b = p.b;
        new_p.u = p.u;
        p = new_p;
    }

    void rla()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::msb(data);
        data <<= 1;
        data |= static_cast<std::uint8_t>(p.c);
        write_byte(addr, data);
        a &= data;
        p.c = carry;
        set_z_and_n(a);
    }

    void rol()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::msb(data);
        data <<= 1;
        data |= static_cast<std::uint8_t>(p.c);
        write_byte(addr, data);
        p.c = carry;
        set_z_and_n(data);
    }

    void rol_a()
    {
        read_byte(pc);
        const auto carry = bit::msb(a);
        a <<= 1;
        a |= static_cast<std::uint8_t>(p.c);
        p.c = carry;
        set_z_and_n(a);
    }

    void ror()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::lsb(data);
        data >>= 1;
        data |= p.c << 7;
        write_byte(addr, data);
        p.c = carry;
        set_z_and_n(data);
    }

    void ror_a()
    {
        read_byte(pc);
        const auto carry = bit::lsb(a);
        a >>= 1;
        a |= p.c << 7;
        p.c = carry;
        set_z_and_n(a);
    }

    void rra()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::lsb(data);
        data >>= 1;
        data |= p.c << 7;
        write_byte(addr, data);
        p.c = carry;
        add(data);
    }

    void rti()
    {
        read_byte(pc);
        peek();
        auto new_p = std::bit_cast<Status>(pop());
        new_p.b = p.b;
        new_p.u = p.u;
        p = new_p;
        const auto pc_low = pop();
        const auto pc_high = pop();
        pc = bit::combine(pc_high, pc_low);
    }

    void rts()
    {
        read_byte(pc);
        peek();
        const auto pc_low = pop();
        const auto pc_high = pop();
        pc = bit::combine(pc_high, pc_low);
        next_byte();
    }

    void sax()
    {
        write_byte(addr, a & x);
    }

    void sbc()
    {
        const auto data = read_byte(addr);
        add(data ^ 0xFF);
    }

    void sbx()
    {
        const auto data = read_byte(addr);
        const auto overflow = num::overflowing_sub(a & x, data, &x);
        p.c = !overflow;
        set_z_and_n(x);
    }

    void sec()
    {
        read_byte(pc);
        p.c = true;
    }

    void sed()
    {
        read_byte(pc);
        p.d = true;
    }

    void sei()
    {
        read_byte(pc);
        p.i = true;
    }

    void sha()
    {
        sh(a & x);
    }

    void shx()
    {
        sh(x);
    }

    void shy()
    {
        sh(y);
    }

    void slo()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::msb(data);
        data <<= 1;
        write_byte(addr, data);
        a |= data;
        p.c = carry;
        set_z_and_n(a);
    }

    void sre()
    {
        auto data = read_byte(addr);
        write_byte(addr, data);
        const auto carry = bit::lsb(data);
        data >>= 1;
        write_byte(addr, data);
        a ^= data;
        p.c = carry;
        set_z_and_n(a);
    }

    void sta()
    {
        write_byte(addr, a);
    }

    void stx()
    {
        write_byte(addr, x);
    }

    void sty()
    {
        write_byte(addr, y);
    }

    void shs()
    {
        s = a & x;
        sh(s);
    }

    void tax()
    {
        read_byte(pc);
        x = a;
        set_z_and_n(x);
    }

    void tay()
    {
        read_byte(pc);
        y = a;
        set_z_and_n(y);
    }

    void tsx()
    {
        read_byte(pc);
        x = s;
        set_z_and_n(x);
    }

    void txa()
    {
        read_byte(pc);
        a = x;
        set_z_and_n(a);
    }

    void txs()
    {
        read_byte(pc);
        s = x;
    }

    void tya()
    {
        read_byte(pc);
        a = y;
        set_z_and_n(a);
    }
};

} // namespace zcnes
