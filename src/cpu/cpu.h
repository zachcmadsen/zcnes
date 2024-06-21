#pragma once

#include <concepts>
#include <cstdint>

namespace zcnes {

template <typename T>
concept Addressable = requires(T t, std::uint16_t addr, std::uint8_t data) {
    { t.Read(addr) } -> std::same_as<std::uint8_t>;
    { t.Write(addr, data) } -> std::same_as<void>;
};

struct Status {
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

template <Addressable T> class Cpu {
  public:
    std::uint16_t pc{0};
    std::uint8_t a{0};
    std::uint8_t x{0};
    std::uint8_t y{0};
    std::uint8_t s{0xFD};
    Status p{false, false, true, false, false, true, false, false};

    explicit Cpu(T &bus) : bus{bus} {};

    void Step() {
        const auto opcode = bus.Read(pc++);
        if (opcode == 0xA5) {
            Zpg();
            Lda();
        } else if (opcode == 0xAD) {
            Abs();
            Lda();
        } else if (opcode == 0xB5) {
            Zpx();
            Lda();
        } else if (opcode == 0xB6) {
            Zpy();
            Ldx();
        } else if (opcode == 0xBD) {
            Abx<false>();
            Lda();
        } else if (opcode == 0xB9) {
            Aby<false>();
            Lda();
        }
    }

  private:
    T &bus;

    std::uint16_t effective_addr{0};

    void Abs() {
        const auto low = bus.Read(pc++);
        const auto high = bus.Read(pc++);
        effective_addr = static_cast<std::uint16_t>(low | high << 8);
    }

    template <bool write> void Abx() {
        std::uint8_t low = bus.Read(pc++);
        const auto overflow = __builtin_add_overflow(low, x, &low);
        const auto high = bus.Read(pc++);
        if (write || overflow) {
            bus.Read(static_cast<std::uint16_t>(low | high << 8));
        }
        effective_addr = (low | (high + overflow) << 8);
    }

    template <bool write> void Aby() {
        std::uint8_t low = bus.Read(pc++);
        const auto overflow = __builtin_add_overflow(low, y, &low);
        const auto high = bus.Read(pc++);
        if (write || overflow) {
            bus.Read(static_cast<std::uint16_t>(low | high << 8));
        }
        effective_addr =
            static_cast<std::uint16_t>(low | (high + overflow) << 8);
    }

    void Zpg() {
        effective_addr = bus.Read(pc++);
    }

    void Zpx() {
        effective_addr = bus.Read(pc++);
        bus.Read(effective_addr);
        effective_addr = static_cast<std::uint8_t>(effective_addr + x);
    }

    void Zpy() {
        effective_addr = bus.Read(pc++);
        bus.Read(effective_addr);
        effective_addr = static_cast<std::uint8_t>(effective_addr + y);
    }

    void Lda() {
        a = bus.Read(effective_addr);
        p.z = a == 0;
        p.n = a & 0x80;
    }

    void Ldx() {
        x = bus.Read(effective_addr);
        p.z = x == 0;
        p.n = x & 0x80;
    }
};

}
