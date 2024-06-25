#include "bus.hpp"

#include <cstdint>

#include "cart.hpp"

namespace zcnes
{

std::uint16_t mirror_ram_addr(std::uint16_t addr)
{
    return addr & 0x07FF;
}

Bus::Bus(Cart *cart) : cart{cart}
{
}

std::uint8_t Bus::read_byte(std::uint16_t addr)
{
    if (addr <= 0x1FFF)
    {
        return ram[mirror_ram_addr(addr)];
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        return cart->read_prg_ram(addr);
    }
    else if (addr >= 0x8000)
    {
        return cart->read_prg_rom(addr);
    }

    return 0;
}

void Bus::write_byte(std::uint16_t addr, std::uint8_t data)
{
    if (addr <= 0x1FFF)
    {
        ram[mirror_ram_addr(addr)] = data;
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        cart->write_prg_ram(addr, data);
    }
}

std::uint8_t Bus::peek_byte(std::uint16_t addr) const
{
    if (addr <= 0x1FFF)
    {
        return ram[mirror_ram_addr(addr)];
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        return cart->read_prg_ram(addr);
    }
    else if (addr >= 0x8000)
    {
        return cart->read_prg_rom(addr);
    }

    return 0;
}

} // namespace zcnes
