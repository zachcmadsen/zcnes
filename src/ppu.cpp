#include "ppu.hpp"

#include <bit>
#include <cstdint>
#include <format>
#include <iostream>

#include "bus.hpp"
#include "cpu.hpp"

namespace zcnes
{

constexpr std::uint64_t master_clock_divider = 4;

Ppu::Ppu(Cpu<Bus> *cpu) : cpu{cpu}
{
}

void Ppu::run(std::uint64_t cycles)
{
    while (master_clock + master_clock_divider <= cycles)
    {
        tick();
        master_clock += master_clock_divider;
    }
}

std::uint8_t Ppu::read(std::uint16_t addr, std::uint64_t clocks)
{
    run(clocks);

    switch (addr & 0x2007)
    {
    case 0x2002:
        if (scanline == 241 && cycle == 1)
        {
            suppress_nmi = true;
        }

        cpu_bus = std::bit_cast<std::uint8_t>(status) | (cpu_bus & 0x1F);
        status.vblank = false;
        cpu->pull_nmi(false);

        break;
    default:
        break;
    }

    return cpu_bus;
}

void Ppu::write(std::uint16_t addr, std::uint8_t data, std::uint64_t clocks)
{
    run(clocks);

    cpu_bus = data;

    switch (addr & 0x2007)
    {
    case 0x2000: {
        const auto prev_ctrl = ctrl;
        ctrl = std::bit_cast<Ctrl>(data);

        if (!ctrl.nmi)
        {
            cpu->pull_nmi(false);
        }
        else if (status.vblank && ctrl.nmi)
        {
            cpu->pull_nmi(true);
        }
        break;
    }
    case 0x2001:
        mask = std::bit_cast<Mask>(data);
        break;
    default:
        break;
    }
}

void Ppu::tick()
{
    if (scanline == 241 && cycle == 1)
    {
        std::cout << std::format("actual vblank event at {}\n", master_clock);

        if (!suppress_nmi)
        {
            status.vblank = true;
            if (ctrl.nmi)
            {
                cpu->pull_nmi(true);
            }
        }
        suppress_nmi = false;
    }
    else if (scanline == 261 && cycle == 1)
    {
        status.vblank = false;
        cpu->pull_nmi(false);
    }

    if (scanline == 261 && cycle == 339)
    {
        if (is_odd_frame && rendering_enabled)
        {
            cycle += 1;
        }
        is_odd_frame = !is_odd_frame;
    }

    cycle = (cycle + 1) % 341;
    if (cycle == 0)
    {
        scanline = (scanline + 1) % 262;
    }

    rendering_enabled = mask.show_bg || mask.show_sprites;
}

} // namespace zcnes
