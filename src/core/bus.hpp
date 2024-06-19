#pragma once

#include <array>
#include <cstdint>

namespace zcnes {

class Bus {
  public:
    std::uint8_t read(std::uint16_t addr);

    void write(std::uint16_t addr, std::uint8_t data);

  private:
    std::array<std::uint8_t, 0x800> ram{0};
};

}
