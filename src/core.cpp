#include "core.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <span>

#include <zcnes/core.hpp>

namespace zcnes
{

Core::Core(std::span<const std::uint8_t> rom) : cart{rom}, bus{&cart, &ppu}, cpu{&bus}, ppu{&cpu}
{
    cpu.reset();
}

void Core::step()
{
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
