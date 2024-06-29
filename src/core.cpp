#include "core.hpp"
#include "scheduler.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <span>

#include <zcnes/core.hpp>

namespace zcnes
{

Core::Core(std::span<const std::uint8_t> rom) : cart{rom}, bus{&cart, &ppu, &scheduler}, cpu{&bus}, ppu{&cpu}
{
    scheduler.add(0, EventKind::Reset);
    scheduler.add(327368, EventKind::VBlank);
}

void Core::step()
{
    scheduler.check([this](EventKind event_kind) {
        switch (event_kind)
        {
        case EventKind::Reset:
            this->cpu.reset();
            break;
        case EventKind::VBlank:
            // TODO: Underestimate vblank, then check if it's too soon. If it is reschedule.
            std::cout << std::format("vblank event at {}\n", this->scheduler.ticks());
            this->ppu.run(this->scheduler.ticks());
            scheduler.add(357562, EventKind::VBlank);
            break;
        }
    });

    cpu.step();
}

std::optional<std::uint8_t> Core::peek(std::uint16_t addr) const
{
    return bus.peek_byte(addr);
}

std::unique_ptr<CoreBase> make_core(std::span<const std::uint8_t> rom)
{
    return std::make_unique<Core>(rom);
}

} // namespace zcnes
