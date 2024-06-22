#pragma once

#include <concepts>
#include <cstdint>

#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow) && __has_builtin(__builtin_sub_overflow)
#define ZCNES_HAVE_OVERFLOW_BUILTINS 1
#endif
#endif

namespace zcnes
{

namespace num
{

inline constexpr std::uint16_t combine(std::uint8_t high, std::uint8_t low)
{
    return static_cast<std::uint16_t>(static_cast<unsigned>(high) << 8 | static_cast<unsigned>(low));
}

inline constexpr std::uint8_t wrapping_add(std::uint8_t lhs, std::uint8_t rhs)
{
    return lhs + rhs;
}

inline constexpr bool overflowing_add(std::uint8_t lhs, std::uint8_t rhs, std::uint8_t *res)
{
#ifdef ZCNES_HAVE_OVERFLOW_BUILTINS
    return __builtin_add_overflow(lhs, rhs, res);
#else
    *res = wrapping_add(lhs, rhs);
    return *res < lhs;
#endif
}

} // namespace num

template <typename T>
concept Addressable = requires(T t, std::uint16_t addr, std::uint8_t data) {
    {
        t.read(addr)
    } -> std::same_as<std::uint8_t>;
    {
        t.write(addr, data)
    } -> std::same_as<void>;
};

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

template <Addressable T> class Cpu
{
  public:
    explicit Cpu(T *bus) : bus{bus} {};

    void step()
    {
        const auto opcode = bus->read(pc++);
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
// Make register members public for processor tests.
#ifdef ZCNES_PROCESSOR_TESTS
  public:
#endif
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

    std::uint16_t effective_addr{0};

    void abs()
    {
        const auto low = bus->read(pc++);
        const auto high = bus->read(pc++);
        effective_addr = num::combine(high, low);
    }

    template <bool write> void abx()
    {
        auto low = bus->read(pc++);
        const auto overflow = num::overflowing_add(low, x, &low);
        const auto high = bus->read(pc++);
        if (write || overflow)
        {
            bus->read(num::combine(high, low));
        }
        effective_addr = num::combine(high + overflow, low);
    }

    template <bool write> void aby()
    {
        auto low = bus->read(pc++);
        const auto overflow = num::overflowing_add(low, y, &low);
        const auto high = bus->read(pc++);
        if (write || overflow)
        {
            bus->read(num::combine(high, low));
        }
        effective_addr = num::combine(high + overflow, low);
    }

    void zpg()
    {
        effective_addr = bus->read(pc++);
    }

    void zpx()
    {
        auto addr = bus->read(pc++);
        bus->read(addr);
        effective_addr = num::wrapping_add(addr, x);
    }

    void zpy()
    {
        auto addr = bus->read(pc++);
        bus->read(addr);
        effective_addr = num::wrapping_add(addr, y);
    }

    void lda()
    {
        a = bus->read(effective_addr);
        p.z = a == 0;
        p.n = (a & 0x80) != 0;
    }

    void ldx()
    {
        x = bus->read(effective_addr);
        p.z = x == 0;
        p.n = (x & 0x80) != 0;
    }
};

} // namespace zcnes
