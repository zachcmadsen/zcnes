#include "ppu.hpp"

#include <bit>
#include <cstdint>

#include "bus.hpp"
#include "cpu.hpp"

namespace zcnes
{

constexpr std::uint64_t PPU_MASTER_CLOCK_DIVIDER = 4;

constexpr std::uint16_t CONTROL_ADDR = 0x2000;
constexpr std::uint16_t MASK_ADDR = 0x2001;
constexpr std::uint16_t STATUS_ADDR = 0x2002;
constexpr std::uint16_t SCROLL_ADDR = 0x2005;
constexpr std::uint16_t ADDRESS_ADDR = 0x2006;
constexpr std::uint16_t DATA_ADDR = 0x2007;

Ppu::Ppu(Cpu<Bus> *cpu) : cpu{cpu}
{
}

void Ppu::run(std::uint64_t cpu_master_clock)
{
    while (master_clock + PPU_MASTER_CLOCK_DIVIDER <= cpu_master_clock)
    {
        tick();
        master_clock += PPU_MASTER_CLOCK_DIVIDER;
    }
}

std::uint8_t Ppu::read(std::uint16_t addr)
{
    const auto mirrored_addr = addr & 0x2007;
    switch (mirrored_addr)
    {
    case STATUS_ADDR:
        open_bus = std::bit_cast<std::uint8_t>(status) | (open_bus & 0x1F);
        status.vblank = false;
        cpu->set_nmi_pin(false);

        w = false;

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
    case DATA_ADDR:
        // TODO: The reads work differently (on some models) when reading
        // palette data from 0x3F00 - 0x3FFF. I'm not sure if I need to
        // emulate it.
        //
        // https://www.nesdev.org/wiki/PPU_registers#Reading_palette_RAM
        open_bus = read_buffer;

        // TODO: Fill the read buffer.
        increment_v();
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
    case CONTROL_ADDR: {
        ctrl = std::bit_cast<Ctrl>(data);

        t.nt = ctrl.base_nt_addr;

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
    case MASK_ADDR:
        mask = std::bit_cast<Mask>(data);
        break;
    case SCROLL_ADDR:
        if (!w)
        {
            t.coarse_x_scroll = data >> 3;
            x = data & 0x07;
        }
        else
        {
            t.coarse_y_scroll = data >> 3;
            t.fine_y_scroll = data & 0x07;
        }
        w = !w;
        break;
    case ADDRESS_ADDR:
        if (!w)
        {
            auto raw_t = std::bit_cast<std::uint16_t>(t);
            // Mask out the high byte, except for the MSB.
            raw_t &= 0x40FF;
            const auto high = static_cast<std::uint16_t>((data & 0x3F) << 8);
            raw_t |= high;
            t = std::bit_cast<Address>(raw_t);
            // Zero the MSB of fine_y_scroll.
            t.fine_y_scroll = t.fine_y_scroll & 0x03;
        }
        else
        {
            auto raw_t = std::bit_cast<std::uint16_t>(t);
            raw_t &= 0xFF00;
            raw_t |= data;
            t = std::bit_cast<Address>(raw_t);

            v = t;
        }
        w = !w;
        break;
    case DATA_ADDR:
        // TODO: Write the data.
        increment_v();
        break;
    default:
        break;
    }
}

void Ppu::tick()
{
    if (scanline < 240 && cycle < 256)
    {
        // Paint the screen red every frame.
        auto pixel_index = scanline * 256 * 4 + (cycle) * 4;
        pixels[pixel_index] = 0xFF;
        pixels[pixel_index + 3] = 0xFF;
    }
    else if (scanline == 241 && cycle == 1)
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
    else if (scanline == 261 && cycle == 0)
    {
        if (on_frame)
        {
            on_frame(pixels);
        }
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

    // Some games (and test ROMs) rely on a one cycle delay for rendering flag
    // updates.
    //
    // https://forums.nesdev.org/viewtopic.php?t=18325
    rendering_enabled = mask.show_bg || mask.show_sprites;
}

void Ppu::increment_v()
{
    auto raw_v = std::bit_cast<std::uint16_t>(v);
    raw_v += ctrl.vram_addr_inc ? 32 : 1;
    v = std::bit_cast<Address>(raw_v);
}

} // namespace zcnes
