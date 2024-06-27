#include "ppu.hpp"

#include <bit>
#include <cstdint>

#include "scheduler.hpp"

namespace zcnes
{

Ppu::Ppu(Scheduler *scheduler) : scheduler{scheduler}
{
}

void Ppu::run()
{
    const auto ticks = scheduler->ticks();
    const auto ticks_to_run = (ticks - prev_ticks) * 3;
    for (int i = 0; i < ticks_to_run; ++i)
    {
        tick();
    }
    prev_ticks = ticks;
}

std::uint8_t Ppu::read(std::uint16_t addr)
{
    run();

    switch (addr & 0x2007)
    {
    case 0x2002:
        cpu_bus = std::bit_cast<std::uint8_t>(status) | (cpu_bus & 0x1F);
        status.vblank = false;
        break;
    default:
        return 0;
    }

    return cpu_bus;
}

void Ppu::write(std::uint16_t addr, std::uint8_t data)
{
    run();

    cpu_bus = data;

    switch (addr & 0x2007)
    {
    case 0x2000: {
        const auto prev_ctrl = ctrl;
        ctrl = std::bit_cast<Ctrl>(data);

        if (status.vblank && !prev_ctrl.nmi && ctrl.nmi)
        {
            scheduler->add(EventKind::Nmi);
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
    if (scanline == 261 && cycle == 1)
    {
        status.vblank = false;
    }

    if (scanline == 241 && cycle == 1)
    {
        status.vblank = true;
        if (ctrl.nmi)
        {
            scheduler->add(EventKind::Nmi);
        }
    }

    if (scanline == 261 && cycle == 340)
    {
        if (mask.show_bg && mask.show_sprites && !is_odd_frame)
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
}

} // namespace zcnes
