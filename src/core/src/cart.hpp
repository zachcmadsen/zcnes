#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace zcnes
{

class Cart
{
  public:
    explicit Cart(std::span<const std::uint8_t> rom);

    std::uint8_t read_prg_ram(std::uint16_t addr);

    std::uint8_t read_prg_rom(std::uint16_t addr);

    void write_prg_ram(std::uint16_t addr, std::uint8_t data);

  private:
    static constexpr std::size_t prg_ram_size = 8192;

    std::array<std::uint8_t, prg_ram_size> prg_ram{};
    std::vector<std::uint8_t> prg_rom{};
};

} // namespace zcnes
