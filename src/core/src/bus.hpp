#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "cart.hpp"

namespace zcnes
{

class Ppu;

class Bus
{
  public:
    explicit Bus(Cart *cart, Ppu *ppu);

    std::uint8_t read_byte(std::uint16_t addr);

    void write_byte(std::uint16_t addr, std::uint8_t data);

    [[nodiscard]] std::optional<std::uint8_t> peek_byte(std::uint16_t addr) const;

  private:
    std::array<std::uint8_t, 0x800> ram{};
    Cart *cart;
    Ppu *ppu;

    std::uint64_t master_clock{2};
};

} // namespace zcnes
