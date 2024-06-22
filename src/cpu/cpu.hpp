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
    void step()
    {
        const auto opcode = next_byte();
        if (opcode == 0xA5)
        {
            zpg();
            lda();
        }
        else if (opcode == 0xAD)
        {
            abs();
            lda();
        }
        else if (opcode == 0xB5)
        {
            zpx();
            lda();
        }
        else if (opcode == 0xB6)
        {
            zpy();
            ldx();
        }
        else if (opcode == 0xBD)
        {
            abx<false>();
            lda();
        }
        else if (opcode == 0xB9)
        {
            aby<false>();
            lda();
        }
    }

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

    void lda()
    {
        a = bus->read_byte(addr);
        p.z = a == 0;
        p.n = (a & 0x80) != 0;
    }

    void ldx()
    {
        x = bus->read_byte(addr);
        p.z = x == 0;
        p.n = (x & 0x80) != 0;
    }
};

} // namespace zcnes
