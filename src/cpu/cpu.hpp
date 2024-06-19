#pragma once

#include <concepts>
#include <cstdint>

namespace zcnes {

template <typename T>
concept Addressable = requires(T t, std::uint16_t addr, std::uint8_t data) {
    { t.read(addr) } -> std::same_as<std::uint8_t>;
    { t.write(addr, data) } -> std::same_as<void>;
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

    explicit Cpu(T &bus) : bus(bus){};

    void step() {
        const auto opc = bus.read(pc++);
        if (opc == 0xA5) {
            zpg();
            lda();
        } else if (opc == 0xAD) {
            abs();
            lda();
        } else if (opc == 0xB5) {
            zpx();
            lda();
        } else if (opc == 0xB6) {
            zpy();
            ldx();
        } else if (opc == 0xBD) {
            abx<false>();
            lda();
        } else if (opc == 0xB9) {
            aby<false>();
            lda();
        }
    }

  private:
    T &bus;

    std::uint16_t addr{0};

    void abs() {
        const auto low = bus.read(pc++);
        const auto high = bus.read(pc++);
        addr = static_cast<std::uint16_t>(low | high << 8);
    }

    template <bool write> void abx() {
        std::uint8_t low = bus.read(pc++);
        const auto overflow = __builtin_add_overflow(low, x, &low);
        const auto high = bus.read(pc++);
        if (write || overflow) {
            bus.read(static_cast<std::uint16_t>(low | high << 8));
        }
        addr = static_cast<std::uint16_t>(low | (high + overflow) << 8);
    }

    template <bool write> void aby() {
        std::uint8_t low = bus.read(pc++);
        const auto overflow = __builtin_add_overflow(low, y, &low);
        const auto high = bus.read(pc++);
        if (write || overflow) {
            bus.read(static_cast<std::uint16_t>(low | high << 8));
        }
        addr = static_cast<std::uint16_t>(low | (high + overflow) << 8);
    }

    void zpg() {
        addr = bus.read(pc++);
    }

    void zpx() {
        addr = bus.read(pc++);
        bus.read(addr);
        addr = static_cast<std::uint8_t>(addr + x);
    }

    void zpy() {
        addr = bus.read(pc++);
        bus.read(addr);
        addr = static_cast<std::uint8_t>(addr + y);
    }

    void lda() {
        a = bus.read(addr);
        p.z = a == 0;
        p.n = a & 0x80;
    }

    void ldx() {
        x = bus.read(addr);
        p.z = x == 0;
        p.n = x & 0x80;
    }
};

}
