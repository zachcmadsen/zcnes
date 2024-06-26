#include "bus.hpp"

#include <cstdint>
#include <optional>

#include "cart.hpp"
#include "scheduler.hpp"

namespace zcnes
{

std::uint16_t mirror_ram_addr(std::uint16_t addr)
{
    return addr & 0x07FF;
}

Bus::Bus(Cart *cart, Scheduler *scheduler) : cart{cart}, scheduler{scheduler}
{
}

std::uint8_t Bus::read_byte(std::uint16_t addr)
{
    scheduler->tick();

    std::uint8_t data = 0;

    if (addr <= 0x1FFF)
    {
        data = ram[mirror_ram_addr(addr)];
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        data = cart->read_prg_ram(addr);
    }
    else if (addr >= 0x8000)
    {
        data = cart->read_prg_rom(addr);
    }

    return data;
}

void Bus::write_byte(std::uint16_t addr, std::uint8_t data)
{
    scheduler->tick();

    if (addr <= 0x1FFF)
    {
        ram[mirror_ram_addr(addr)] = data;
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        cart->write_prg_ram(addr, data);
    }
}

std::optional<std::uint8_t> Bus::peek_byte(std::uint16_t addr) const
{
    std::optional<std::uint8_t> data{};

    if (addr <= 0x1FFF)
    {
        data = ram[mirror_ram_addr(addr)];
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        data = cart->read_prg_ram(addr);
    }
    else if (addr >= 0x8000)
    {
        data = cart->read_prg_rom(addr);
    }

    return data;
}

} // namespace zcnes
