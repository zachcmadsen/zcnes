#include "ppu.hpp"

#include <bit>
#include <cstdint>

#include <cpu/cpu.hpp>

#include "bus.hpp"

namespace zcnes
{

constexpr std::uint64_t ppu_master_clock_divider = 4;

constexpr std::uint16_t control_addr = 0x2000;
constexpr std::uint16_t mask_addr = 0x2001;
constexpr std::uint16_t status_addr = 0x2002;

Ppu::Ppu(Cpu<Bus> *cpu) : cpu{cpu}
{
}

void Ppu::run(std::uint64_t cpu_master_clock)
{
    while (master_clock + ppu_master_clock_divider <= cpu_master_clock)
    {
        tick();
        master_clock += ppu_master_clock_divider;
    }
}

std::uint8_t Ppu::read(std::uint16_t addr)
{
    const auto mirrored_addr = addr & 0x2007;
    switch (mirrored_addr)
    {
    case status_addr:
        open_bus = std::bit_cast<std::uint8_t>(status) | (open_bus & 0x1F);
        status.vblank = false;
        cpu->set_nmi_pin(false);

        // Reading from the status register on the same cycle (and the one
        // after?) that the vblank flag is set suppresses the NMI for that
        // frame.
        //
        // https://www.nesdev.org/wiki/PPU_frame_timing
        if (scanline == 241 && cycle == 1)
        {
            suppress_nmi = true;
        }
        break;
    default:
        break;
    }
    return open_bus;
}

void Ppu::write(std::uint16_t addr, std::uint8_t data)
{
    open_bus = data;

    const auto mirrored_addr = addr & 0x2007;
    switch (mirrored_addr)
    {
    case control_addr: {
        ctrl = std::bit_cast<Ctrl>(data);

        if (!ctrl.nmi)
        {
            cpu->set_nmi_pin(false);
        }
        else if (status.vblank && ctrl.nmi)
        {
            cpu->set_nmi_pin(true);
        }
        break;
    }
    case mask_addr:
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
        if (!suppress_nmi)
        {
            status.vblank = true;
            if (ctrl.nmi)
            {
                cpu->set_nmi_pin(true);
            }
        }
        suppress_nmi = false;
    }
    else if (scanline == 261 && cycle == 1)
    {
        status.vblank = false;
        cpu->set_nmi_pin(false);
    }
    else if (scanline == 261 && cycle == 339)
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

    // Some games (and test ROMs) rely on a one cycle delay of rendering flag
    // updates.
    //
    // https://forums.nesdev.org/viewtopic.php?t=18325
    rendering_enabled = mask.show_bg || mask.show_sprites;
}

} // namespace zcnes
