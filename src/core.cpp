#include "core.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <span>

#include <zcnes/core.hpp>

#include "scheduler.hpp"

namespace zcnes
{

Core::Core(std::span<const std::uint8_t> rom) : cart{rom}, bus{&cart, &scheduler}, cpu{&bus}
{
    scheduler.add(0, EventKind::Reset);
}

void Core::step()
{
    scheduler.check([this](EventKind event_kind) {
        switch (event_kind)
        {
        case EventKind::Reset:
            this->cpu.reset();
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
