#pragma once

#include <concepts>
#include <cstdint>

#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow)
#define ZCNES_USE_OVERFLOW_BUILTIN 1
#endif
#endif

namespace zcnes
{

namespace num
{

/// Combines `high` and `low` into a 16-bit integer.
///
/// The implementation is copied from https://stackoverflow.com/a/57320436.
inline constexpr std::uint16_t combine(std::uint8_t high, std::uint8_t low)
{
    return static_cast<std::uint16_t>(static_cast<unsigned>(high) << 8 | static_cast<unsigned>(low));
}

/// Computes `lhs` + `rhs`, wrapping at the max value of `std::uint8_t`.
inline constexpr std::uint8_t wrapping_add(std::uint8_t lhs, std::uint8_t rhs)
{
    return lhs + rhs;
}

/// Computes `lhs` + `rhs` and returns a boolean indicating whether overflow
/// occurred.
inline constexpr bool overflowing_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
#ifdef ZCNES_USE_OVERFLOW_BUILTIN
    return __builtin_add_overflow(lhs, rhs, res);
#else
    *res = wrapping_add(lhs, rhs);
    return *res < lhs;
#endif
}

} // namespace num

/// A type that can be addressed to read and write bytes.
template <typename T>
concept Addressable = requires(T t, std::uint16_t addr, std::uint8_t data) {
    {
        t.read_byte(addr)
    } -> std::same_as<std::uint8_t>;
    {
        t.write_byte(addr, data)
    } -> std::same_as<void>;
};

/// A Ricoh 6502 emulator.
template <Addressable T> class Cpu
{
  public:
    explicit Cpu(T *bus) : bus{bus} {};

    /// Executes the next instruction.
    void step();

  private:
// Register members need to be public for processor tests.
#ifdef ZCNES_PROCESSOR_TESTS
  public:
#endif
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
#ifdef ZCNES_PROCESSOR_TESTS
  private:
#endif

    T *bus;

    std::uint16_t addr{0};

    std::uint8_t next_byte()
    {
        const auto data = bus->read_byte(pc);
        pc += 1;
        return data;
    }

    void set_z_and_n(std::uint8_t data)
    {
        p.z = data == 0;
        p.n = (data & 0x80) != 0;
    }

    void abs()
    {
        const auto low = next_byte();
        const auto high = next_byte();
        addr = num::combine(high, low);
    }

    template <bool write> void abx()
    {
        auto low = next_byte();
        const auto overflow = num::overflowing_add(low, x, &low);
        const auto high = next_byte();
        if (write || overflow)
        {
            bus->read_byte(num::combine(high, low));
        }
        addr = num::combine(high + overflow, low);
    }

    template <bool write> void aby()
    {
        auto low = next_byte();
        const auto overflow = num::overflowing_add(low, y, &low);
        const auto high = next_byte();
        if (write || overflow)
        {
            bus->read_byte(num::combine(high, low));
        }
        addr = num::combine(high + overflow, low);
    }

    void idx()
    {
    }

    template <bool write> void idy()
    {
    }

    void imm()
    {
    }

    void imp()
    {
    }

    void ind()
    {
    }

    void zpg()
    {
        addr = next_byte();
    }

    void zpx()
    {
        const auto addr = next_byte();
        bus->read_byte(addr);
        this->addr = num::wrapping_add(addr, x);
    }

    void zpy()
    {
        const auto addr = next_byte();
        bus->read_byte(addr);
        this->addr = num::wrapping_add(addr, y);
    }

    void adc()
    {
    }

    void alr()
    {
    }

    void anc()
    {
    }

    void andd()
    {
    }

    void ane()
    {
    }

    void arr()
    {
    }

    void asl()
    {
    }

    void asl_a()
    {
    }

    void bcc()
    {
    }
    void bcs()
    {
    }

    void beq()
    {
    }

    void bit()
    {
    }

    void bmi()
    {
    }

    void bne()
    {
    }

    void bpl()
    {
    }

    void brk()
    {
    }

    void bvc()
    {
    }

    void bvs()
    {
    }

    void clc()
    {
    }

    void cld()
    {
    }

    void cli()
    {
    }

    void clv()
    {
    }

    void cmp()
    {
    }

    void cpx()
    {
    }

    void cpy()
    {
    }

    void dcp()
    {
    }

    void dec()
    {
    }

    void dex()
    {
    }

    void dey()
    {
    }

    void eor()
    {
    }

    void inc()
    {
    }

    void inx()
    {
    }

    void iny()
    {
    }

    void isc()
    {
    }

    void jam()
    {
    }

    void jmp()
    {
    }

    void jsr()
    {
    }

    void las()
    {
    }

    void lax()
    {
    }

    void lda()
    {
        a = bus->read_byte(addr);
        set_z_and_n(a);
    }

    void ldx()
    {
        x = bus->read_byte(addr);
        set_z_and_n(x);
    }

    void ldy()
    {
        y = bus->read_byte(addr);
        set_z_and_n(y);
    }

    void lsr()
    {
    }

    void lsr_a()
    {
    }

    void lxa()
    {
    }

    void nop()
    {
    }

    void ora()
    {
    }

    void pha()
    {
    }

    void php()
    {
    }

    void pla()
    {
    }

    void plp()
    {
    }

    void rla()
    {
    }

    void rol()
    {
    }

    void rol_a()
    {
    }

    void ror()
    {
    }

    void ror_a()
    {
    }

    void rra()
    {
    }

    void rti()
    {
    }

    void rts()
    {
    }

    void sax()
    {
    }

    void sbc()
    {
    }

    void sbx()
    {
    }

    void sec()
    {
    }

    void sed()
    {
    }

    void sei()
    {
    }

    void sha()
    {
    }

    void shx()
    {
    }

    void shy()
    {
    }

    void slo()
    {
    }

    void sre()
    {
    }

    void sta()
    {
    }

    void stx()
    {
    }

    void sty()
    {
    }

    void tas()
    {
    }

    void tax()
    {
    }

    void tay()
    {
    }

    void tsx()
    {
    }

    void txa()
    {
    }

    void txs()
    {
    }

    void tya()
    {
    }
};

template <Addressable T> inline void Cpu<T>::step()
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
    case 0x21: idx();        andd();  break;
    case 0x23: idx();        rla();   break;
    case 0x24: zpg();        bit();   break;
    case 0x25: zpg();        andd();  break;
    case 0x26: zpg();        rol();   break;
    case 0x27: zpg();        rla();   break;
    case 0x28:               plp();   break;
    case 0x29: imm();        andd();  break;
    case 0x2A:               rol_a(); break;
    case 0x2B: imm();        anc();   break;
    case 0x2C: abs();        bit();   break;
    case 0x2D: abs();        andd();  break;
    case 0x2E: abs();        rol();   break;
    case 0x2F: abs();        rla();   break;
    case 0x30:               bmi();   break;
    case 0x31: idy<read>();  andd();  break;
    case 0x33: idy<write>(); rla();   break;
    case 0x34: zpx();        nop();   break;
    case 0x35: zpx();        andd();  break;
    case 0x36: zpx();        rol();   break;
    case 0x37: zpx();        rla();   break;
    case 0x38:               sec();   break;
    case 0x39: aby<read>();  andd();  break;
    case 0x3A: imp();        nop();   break;
    case 0x3B: aby<write>(); rla();   break;
    case 0x3C: abx<read>();  nop();   break;
    case 0x3D: abx<read>();  andd();  break;
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
    case 0x9B: aby<write>(); tas();   break;
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
    case 0xAB:                        break;
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
}

} // namespace zcnes
