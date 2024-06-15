#pragma once

#include <concepts>
#include <cstdint>

namespace zcnes {

template <typename T>
concept Bus = requires(T t, std::uint16_t addr, std::uint8_t data) {
    { t.read(addr) } -> std::same_as<std::uint8_t>;
    { t.write(addr, data) } -> std::same_as<void>;
};

struct StatusFlags {
    bool c{false};
    bool z{false};
    bool i{true};
    bool d{false};
    bool b{false};
    bool v{false};
    bool n{false};

    [[nodiscard]] std::uint8_t to_byte() const {
        return static_cast<std::uint8_t>(c | z << 1 | i << 2 | d << 3 | b << 4 |
                                         1 << 5 | v << 6 | n << 7);
    }

    static StatusFlags from_byte(std::uint8_t byte) {
        return {static_cast<bool>(byte & 0x01), static_cast<bool>(byte & 0x02),
                static_cast<bool>(byte & 0x04), static_cast<bool>(byte & 0x08),
                static_cast<bool>(byte & 0x10), static_cast<bool>(byte & 0x40),
                static_cast<bool>(byte & 0x80)};
    }
};

template <Bus T> class Cpu {
  public:
    std::uint8_t a{0};
    std::uint8_t x{0};
    std::uint8_t y{0};
    std::uint8_t s{0xFD};
    std::uint16_t pc{0};
    StatusFlags p{};

    explicit Cpu(T &bus) : bus(bus) {};

    void step() {
        const auto opc = bus.read(pc++);
        if (opc == 0xA5) {
            zpg();
            lda();
        }
    }

  private:
    T &bus;

    std::uint16_t addr{0};

    void zpg() {
        addr = bus.read(pc++);
    }

    void lda() {
        a = bus.read(addr);
        p.z = a == 0;
        p.n = a & 0x80;
    }
};

}
