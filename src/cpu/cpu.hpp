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
