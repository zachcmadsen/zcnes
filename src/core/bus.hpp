#pragma once

#include <array>
#include <cstdint>

namespace zcnes {

class Bus {
  public:
    std::uint8_t Read(std::uint16_t addr);

    void Write(std::uint16_t addr, std::uint8_t data);

  private:
    std::array<std::uint8_t, 0x800> ram{};
};

}
