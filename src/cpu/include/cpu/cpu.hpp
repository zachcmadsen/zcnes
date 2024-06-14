#pragma once

#include <concepts>
#include <cstdint>

namespace zcnes {

template <typename T>
concept Bus = requires(T t, std::uint16_t addr, std::uint8_t data) {
    { t.read(addr) } -> std::same_as<std::uint8_t>;
    { t.write(addr, data) } -> std::same_as<void>;
};

template <Bus T> class Cpu {
  public:
    Cpu(T &bus) : bus(bus) {};

    void step() {
        const auto opc = bus.read(pc++);
        if (opc == 0xA5) {
            zpg();
            lda();
        }
    }

    std::uint8_t a{0};
    std::uint8_t x{0};
    std::uint8_t y{0};
    std::uint16_t pc{0};
    std::uint8_t s{0xFD};

  private:
    T &bus;

    std::uint16_t addr;

    void zpg() {
        addr = bus.read(pc++);
    }

    void lda() {
        a = bus.read(addr);
    }
};

}
