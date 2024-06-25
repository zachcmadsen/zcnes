#include "cart.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>

namespace zcnes
{

constexpr std::size_t header_size = 16;
constexpr std::size_t prg_rom_bank_count_offset = 4;
constexpr std::size_t prg_rom_bank_size = 16384;

Cart::Cart(std::span<const std::uint8_t> rom)
{
    const auto header = rom.first<header_size>();
    const auto prg_rom_bank_count = header[prg_rom_bank_count_offset];

    const auto prg_rom_size = prg_rom_bank_count * prg_rom_bank_size;
    prg_rom.resize(prg_rom_size);
    std::ranges::copy(rom.subspan(header_size, prg_rom_size), prg_rom.data());
}

std::uint8_t Cart::read_prg_ram(std::uint16_t addr)
{
    return prg_ram[addr - 0x6000];
}

std::uint8_t Cart::read_prg_rom(std::uint16_t addr)
{
    return prg_rom[(addr - 0x8000) % prg_rom.size()];
}

void Cart::write_prg_ram(std::uint16_t addr, std::uint8_t data)
{
    prg_ram[addr - 0x6000] = data;
}

} // namespace zcnes
